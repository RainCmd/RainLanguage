using System;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;

namespace LanguageServer
{
    internal static class Reflector
    {
        private static bool TryGetParameterType(MethodInfo method, out Type? parameterType, out bool hasToken)
        {
            parameterType = null;
            hasToken = false;
            var parameters = method.GetParameters();
            foreach (var param in parameters)
                if (param.IsIn || param.IsOut)
                    return false;
            if (parameters.Length == 0) return true;
            else if (parameters.Length == 1)
            {
                if (parameters[0].ParameterType == typeof(CancellationToken)) hasToken = true;
                else parameterType = parameters[0].ParameterType;
                return true;
            }
            else if (parameters.Length == 2)
            {
                if (parameters[1].ParameterType == typeof(CancellationToken))
                {
                    hasToken = true;
                    parameterType = parameters[0].ParameterType;
                    return true;
                }
            }
            return false;
        }
        #region 请求相关反射
        internal static bool IsRequestHandler(MethodInfo method)
        {
            if (!TryGetParameterType(method, out _, out _)) return false;
            var retType = method.ReturnType;
            if (!retType.IsGenericType) return false;
            var openRetType = retType.GetGenericTypeDefinition();
            return openRetType == typeof(Result<,>) || openRetType == typeof(VoidResult<>);
        }
        [RequiresDynamicCode("Calls System.Type.MakeGenericType(params Type[])")]
        internal static Type GetRequestType(MethodInfo method)
        {
            if (TryGetParameterType(method, out var type, out _))
            {
                if (type == null) return typeof(VoidRequestMessage);
                else return typeof(RequestMessage<>).MakeGenericType(type);
            }
            throw new ArgumentException($"签名不匹配: {method.Name}");
        }
        [RequiresDynamicCode("Calls System.Type.MakeGenericType(params Type[])")]
        internal static Type GetResponseType(MethodInfo method)
        {
            var retType = method.ReturnType;
            if (!retType.IsGenericType) throw new ArgumentException($"签名不匹配: {method.Name}");
            var openRetType = retType.GetGenericTypeDefinition();
            if (openRetType == typeof(Result<,>)) return typeof(ResponseMessage<,>).MakeGenericType(retType.GenericTypeArguments[0], retType.GenericTypeArguments[1]);
            else if (openRetType == typeof(VoidResult<>)) return typeof(VoidResponseMessage<>).MakeGenericType(retType.GenericTypeArguments[0]);
            throw new ArgumentException($"签名不匹配: {method.Name}");
        }

        private static readonly MethodInfo method_ForRequest3 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest3))!;
        private static RequestHandlerDelegate ForRequest3<TParams, TResult, TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new()
        {
            Func<Connection, TParams?, CancellationToken, Result<TResult, TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<Connection, TParams?, CancellationToken, Result<TResult, TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<Connection, TParams?, Result<TResult, TResponseError>>>();
                func = (connection, param, token) => methodDelegate(connection, param);
            }

            return (request, connection, token) =>
            {
                var requestMessage = (RequestMessage<TParams>)request;
                Result<TResult, TResponseError> result;
                try
                {
                    result = func(connection, requestMessage.@params, token);
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine(e);
                    result = Result<TResult, TResponseError>.Error(Message.InternalError<TResponseError>());
                }
                return new ResponseMessage<TResult, TResponseError>
                {
                    id = requestMessage.id,
                    result = result.SuccessValue,
                    error = result.ErrorValue
                };
            };
        }

        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForRequest3(Type paramsType, Type resultType, Type responseErrorType)
        {
            return method_ForRequest3.MakeGenericMethod(paramsType, resultType, responseErrorType);
        }

        private static readonly MethodInfo method_ForRequest2 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest2))!;
        private static RequestHandlerDelegate ForRequest2<TResult, TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new()
        {
            Func<Connection, CancellationToken, Result<TResult, TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<Connection, CancellationToken, Result<TResult, TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<Connection, Result<TResult, TResponseError>>>();
                func = (connection, token) => methodDelegate(connection);
            }

            return (request, connection, token) =>
            {
                var voidRequest = (VoidRequestMessage)request;
                Result<TResult, TResponseError> result;
                try
                {
                    result = func(connection, token);
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine(e);
                    result = Result<TResult, TResponseError>.Error(Message.InternalError<TResponseError>());
                }
                return new ResponseMessage<TResult, TResponseError>
                {
                    id = voidRequest.id,
                    result = result.SuccessValue,
                    error = result.ErrorValue
                };
            };
        }

        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForRequest2(Type resultType, Type responseErrorType)
        {
            return method_ForRequest2.MakeGenericMethod(resultType, responseErrorType);
        }

        private static readonly MethodInfo method_ForRequest = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest))!;
        private static RequestHandlerDelegate ForRequest<TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new()
        {
            Func<Connection, CancellationToken, VoidResult<TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<Connection, CancellationToken, VoidResult<TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<Connection, VoidResult<TResponseError>>>();
                func = (connection, token) => methodDelegate(connection);
            }

            return (request, connection, token) =>
            {
                var voidRequest = (VoidRequestMessage)request;
                VoidResult<TResponseError> result;
                try
                {
                    result = func(connection, token);
                }
                catch (Exception e)
                {
                    Console.Error.WriteLine(e);
                    result = VoidResult<TResponseError>.Error(Message.InternalError<TResponseError>());
                }
                return new VoidResponseMessage<TResponseError>
                {
                    id = voidRequest.id,
                    error = result.ErrorValue
                };
            };
        }

        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForRequest(Type responseErrorType)
        {
            return method_ForRequest.MakeGenericMethod(responseErrorType);
        }

        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForRequest2(MethodInfo, Type, Type)")]
        internal static RequestHandlerDelegate CreateRequestHandlerDelegate(MethodInfo method)
        {
            var parameters = method.GetParameters();
            if (parameters.Length > 2) throw new ArgumentException($"签名不匹配: {method.Name}");
            Type? paramsType = (parameters.Length == 1) ? parameters[0].ParameterType : null;
            var returnType = method.ReturnType;
            Type? resultType;
            Type responseErrorType;
            if (returnType.IsGenericType)
            {
                var genericType = returnType.GetGenericTypeDefinition();
                if (genericType == typeof(Result<,>))
                {
                    resultType = returnType.GenericTypeArguments[0];
                    responseErrorType = returnType.GenericTypeArguments[1];
                }
                else if (genericType == typeof(VoidResult<>))
                {
                    resultType = null;
                    responseErrorType = returnType.GenericTypeArguments[0];
                }
                else throw new ArgumentException($"签名不匹配: {method.Name}");
            }
            else throw new ArgumentException($"签名不匹配: {method.Name}");

            var factory =
                (paramsType != null && resultType != null) ? GetFactoryForRequest3(paramsType, resultType, responseErrorType) :
                (paramsType == null && resultType != null) ? GetFactoryForRequest2(resultType, responseErrorType) :
                GetFactoryForRequest(responseErrorType);
            return (RequestHandlerDelegate)factory.Invoke(null, [method])!;
        }
        #endregion

        #region 消息相关反射
        internal static bool IsNotificationHandler(MethodInfo method)
        {
            return TryGetParameterType(method, out _, out var hasToken) && !hasToken && method.ReturnType == typeof(void);
        }

        [RequiresDynamicCode("Calls System.Type.MakeGenericType(params Type[])")]
        internal static Type GetNotificationType(MethodInfo method)
        {
            if (TryGetParameterType(method, out var type, out var hasToken) && !hasToken)
            {
                if (type == null) return typeof(VoidNotificationMessage);
                else return typeof(NotificationMessage<>).MakeGenericType(type);
            }
            throw new ArgumentException($"签名不匹配: {method.Name}");
        }
        private static readonly MethodInfo method_ForNotification2 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForNotification2))!;
        private static NotificationHandlerDelegate ForNotification2<TParams>(MethodInfo method)
        {
            var action = method.CreateDelegate<Action<Connection, TParams?>>();

            return (notification, connection) =>
            {
                var notificationMessage = (NotificationMessage<TParams>)notification;
                try
                {
                    action(connection, notificationMessage.@params);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            };
        }
        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForNotification2(Type paramsType)
        {
            return method_ForNotification2.MakeGenericMethod(paramsType);
        }
        private static NotificationHandlerDelegate ForNotification(MethodInfo method)
        {
            var action = method.CreateDelegate<Action<Connection>>();

            return (notification, connection) =>
            {
                try
                {
                    action(connection);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            };
        }
        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForNotification2(MethodInfo, Type, Type)")]
        internal static NotificationHandlerDelegate CreateNotificationHandlerDelegate(MethodInfo method)
        {
            if (method.ReturnType != typeof(void))
            {
                throw new ArgumentException($"signature mismatch: {method.Name}");
            }
            if (!TryGetParameterType(method, out var parameterType, out _))
            {
                throw new ArgumentException($"signature mismatch: {method.Name}");
            }
            if (parameterType == null) return ForNotification(method);
            else return (NotificationHandlerDelegate)GetFactoryForNotification2(parameterType).Invoke(null, [method])!;
        }
        #endregion

        internal static ResponseMessageBase CreateErrorResponse(Type responseType, string errorMessage)
        {
            var res = Activator.CreateInstance(responseType) as ResponseMessageBase;
            var prop = responseType.GetRuntimeProperty("error");
            var err = Activator.CreateInstance(prop.PropertyType) as ResponseError;
            err.code = ErrorCodes.InternalError;
            err.message = errorMessage;
            prop.SetValue(res, err);
            return res;
        }
    }
}
