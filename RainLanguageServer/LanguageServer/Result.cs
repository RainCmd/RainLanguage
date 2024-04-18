namespace LanguageServer
{
    public enum ResultTag
    {
        Success = 1,
        Error = 2,
    }

    public class Result<T, TError>(ResultTag tag, T? success, TError? error)
    {
        public static Result<T, TError> Success(T? success)
        {
            return new Result<T, TError>(ResultTag.Success, success, default);
        }

        public static Result<T, TError> Error(TError? error)
        {
            return new Result<T, TError>(ResultTag.Error, default, error);
        }

        public T? SuccessValue => success;
        public TError? ErrorValue => error;
        public bool IsSuccess => tag == ResultTag.Success;
        public bool IsError => tag == ResultTag.Error;

        public Result<TResult, TError> Select<TResult>(Func<T?, TResult> func) => IsSuccess ? Result<TResult, TError>.Success(func(SuccessValue)) : Result<TResult, TError>.Error(ErrorValue);

        public Result<TResult, TErrorResult>? Select<TResult, TErrorResult>(Func<T?, TResult> funcSuccess, Func<TError?, TErrorResult> funcError) =>
            IsSuccess ? Result<TResult, TErrorResult>.Success(funcSuccess(SuccessValue)) :
            IsError ? Result<TResult, TErrorResult>.Error(funcError(ErrorValue)) :
            default;

        public TResult? Handle<TResult>(Func<T?, TResult> funcSuccess, Func<TError?, TResult> funcError) =>
            IsSuccess ? funcSuccess(SuccessValue) :
            IsError ? funcError(ErrorValue) :
            default;

        public void Handle(Action<T?> actionSuccess, Action<TError?> actionError)
        {
            if (IsSuccess) actionSuccess(SuccessValue);
            else if (IsError) actionError(ErrorValue);
        }

        public T? HandleError(Func<TError?, T> func) => IsError ? func(ErrorValue) : SuccessValue;
    }

    public class VoidResult<TError>(ResultTag tag, TError? error)
    {
        public static VoidResult<TError> Success()
        {
            return new VoidResult<TError>(ResultTag.Success, default);
        }

        public static VoidResult<TError> Error(TError error)
        {
            return new VoidResult<TError>(ResultTag.Error, error);
        }

        public TError? ErrorValue => error;
        public bool IsSuccess => tag == ResultTag.Success;
        public bool IsError => tag == ResultTag.Error;

        public Result<TResult, TError> Select<TResult>(Func<TResult> func) => IsSuccess ? Result<TResult, TError>.Success(func()) : Result<TResult, TError>.Error(ErrorValue);

        public Result<TResult, TErrorResult>? Select<TResult, TErrorResult>(Func<TResult> funcSuccess, Func<TError?, TErrorResult> funcError) =>
            IsSuccess ? Result<TResult, TErrorResult>.Success(funcSuccess()) :
            IsError ? Result<TResult, TErrorResult>.Error(funcError(ErrorValue)) :
            default;

        public TResult? Handle<TResult>(Func<TResult> funcSuccess, Func<TError?, TResult> funcError) =>
            IsSuccess ? funcSuccess() :
            IsError ? funcError(ErrorValue) :
            default;

        public void Handle(Action actionSuccess, Action<TError?> actionError)
        {
            if (IsSuccess) actionSuccess();
            else if (IsError) actionError(ErrorValue);
        }

        public void HandleError(Action<TError?> actionError)
        {
            if (IsError) actionError(ErrorValue);
        }
    }
}
