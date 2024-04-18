using LanguageServer.Json;

namespace LanguageServer
{
    public static class Message
    {
        public static Result<T, TError> ToResult<T, TError>(ResponseMessage<T, TError> response) where TError : ResponseError
        {
            return (response.error == null)
                ? Result<T, TError>.Success(response.result)
                : Result<T, TError>.Error(response.error);
        }

        public static VoidResult<TError> ToResult<TError>(VoidResponseMessage<TError> response) where TError : ResponseError
        {
            return (response.error == null)
                ? VoidResult<TError>.Success()
                : VoidResult<TError>.Error(response.error);
        }

        public static ResponseError ParseError() => new() { code = ErrorCodes.ParseError, message = "Parse error" };
        public static ResponseError<T> ParseError<T>(T data) => new() { code = ErrorCodes.ParseError, message = "Parse error", data = data };

        public static ResponseError InvalidRequest() => new() { code = ErrorCodes.InvalidRequest, message = "Invalid Request" };
        public static ResponseError<T> InvalidRequest<T>(T data) => new() { code = ErrorCodes.InvalidRequest, message = "Invalid Request", data = data };

        public static ResponseError MethodNotFound() => new() { code = ErrorCodes.MethodNotFound, message = "Method not found" };
        public static ResponseError<T> MethodNotFound<T>(T data) => new() { code = ErrorCodes.MethodNotFound, message = "Method not found", data = data };

        public static ResponseError InvalidParams() => new() { code = ErrorCodes.InvalidParams, message = "Invalid params" };
        public static ResponseError<T> InvalidParams<T>(T data) => new() { code = ErrorCodes.InvalidParams, message = "Invalid params", data = data };

        public static TResponseError InternalError<TResponseError>() where TResponseError : ResponseError, new() => new() { code = ErrorCodes.InternalError, message = "Internal error" };
        public static ResponseError InternalError() => new() { code = ErrorCodes.InternalError, message = "Internal error" };
        public static ResponseError<T> InternalError<T>(T data) => new() { code = ErrorCodes.InternalError, message = "Internal error", data = data };

        public static ResponseError ServerError(ErrorCodes code) => new() { code = code, message = "Server error" };
        public static ResponseError<T> ServerError<T>(ErrorCodes code, T data) => new() { code = code, message = "Server error", data = data };
    }

    internal class MessageTest(string jsonrpc, NumberOrString id, string method)
    {
        public string jsonrpc = jsonrpc;

        public NumberOrString id = id;

        public string method = method;

        public bool IsMessage => jsonrpc == "2.0";

        public bool IsRequest => IsMessage && id != null && method != null;

        public bool IsResponse => IsMessage && id != null && method == null;

        public bool IsNotification => IsMessage && id == null && method != null;

        public bool IsCancellation => IsNotification && method == "$/cancelRequest";
    }

    public abstract class MessageBase
    {
        public string jsonrpc = "2.0";
    }

    public abstract class MethodCall : MessageBase
    {
        public string? method;
    }

    public abstract class RequestMessageBase : MethodCall
    {
        public NumberOrString? id;
    }

    public class VoidRequestMessage : RequestMessageBase
    {
    }

    public class RequestMessage<T> : RequestMessageBase
    {
        public T? @params;
    }

    public abstract class ResponseMessageBase : MessageBase
    {
        public NumberOrString? id;
    }

    public class VoidResponseMessage<TError> : ResponseMessageBase where TError : ResponseError
    {
        public TError? error;
    }

    public class ResponseMessage<T, TError> : ResponseMessageBase where TError : ResponseError
    {
        public T? result;

        public TError? error;
    }

    public abstract class NotificationMessageBase : MethodCall
    {
    }

    public class VoidNotificationMessage : NotificationMessageBase
    {
    }

    public class NotificationMessage<T> : NotificationMessageBase
    {
        public T? @params;
    }

    public class ResponseError
    {
        public ErrorCodes? code;

        public string? message;
    }

    public class ResponseError<T> : ResponseError
    {
        public T? data;
    }

    public enum ErrorCodes
    {
        ParseError = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams = -32602,
        InternalError = -32603,
        ServerErrorStart = -32099,
        ServerErrorEnd = -32000,
        ServerNotInitialized = -32002,
        UnknownErrorCode = -32001,
        RequestCancelled = -32800,
    }
}
