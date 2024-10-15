#include "Utils/Exceptions.hpp"

namespace Utils
{

ExceptionWithReason::ExceptionWithReason() noexcept
{
}

ExceptionWithReason* ExceptionWithReason::createNextException(
    SimulatorErrors error, const char* reason, std::unique_ptr<ExceptionWithReason>&& prevException
) noexcept
{
    ExceptionWithReason* nextException = nullptr;
    try 
    {
        nextException = new ExceptionWithReason();
    }
    catch (std::bad_alloc)
    {
        fprintf(stderr, "CAUGHT BAD ALLOC WHILE CREATING NEXT EXCEPTION\n. Terminating.\n");
        fprintf(stderr, "%s", nextException->what());

        std::terminate();
    }
    catch (...)
    {
        fprintf(stderr, "UNKNOWN EXCEPTION CAUGHT WHILE CREATING NEXT EXCEPTION\n. Terminating.\n");
        fprintf(stderr, "%s", nextException->what());

        std::terminate();        
    }

    if (!nextException)
        return prevException.release();
    
    nextException->prevException_.reset(prevException.release());
    nextException->reasonString_.reset(strdup(reason));
    nextException->error_ = error;

    return nextException;
}

ExceptionWithReason* ExceptionWithReason::createNextException(
    SimulatorErrors error, const char* reason, std::unique_ptr<ExceptionWithReason>&& prevException,
    const char* funcWithErr, const char* fileWithErr, const size_t lineWithErr
) noexcept
{
    static const size_t bufferForReasonSize = 1024;
    static char extendedReason[bufferForReasonSize] = "";

    snprintf(
        extendedReason, bufferForReasonSize, "%s Occurred in func %s, file %s, line %d", 
        reason, funcWithErr, fileWithErr, lineWithErr
    );
    
    return ExceptionWithReason::createNextException(error, extendedReason, std::move(prevException));
}

const char* ExceptionWithReason::what() const noexcept
{
    const ExceptionWithReason* exceptionNow = this;

    auto& a = exceptionNow->reasonString_;
    
    fprintf(stderr, "Exception occurred. Reasons:\n");

    size_t reasonId = 0;
    const char* reason = nullptr;

    while (exceptionNow)
    {
        reason = exceptionNow->reasonString_.get();

        if (reason) 
        {
            fprintf(
                stderr, "%zu. %s Error code - %d\n", reasonId, reason, static_cast<int>(exceptionNow->error_)
            );
            
            reasonId++;
        }

        exceptionNow = exceptionNow->prevException_.get();
    }

    return "";
}

} // namespace Utils