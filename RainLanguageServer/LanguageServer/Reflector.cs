using LanguageServer.Json;
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

        private static readonly MethodInfo method_ForRequest4 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest4))!;
        private static RequestHandlerDelegate ForRequest4<T, TParams, TResult, TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new() where T : Connection
        {
            Func<T, TParams?, CancellationToken, Result<TResult, TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<T, TParams?, CancellationToken, Result<TResult, TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<T, TParams?, Result<TResult, TResponseError>>>();
                func = (connection, param, token) => methodDelegate(connection, param);
            }

            return (request, connection, token) =>
            {
                var requestMessage = (RequestMessage<TParams>)request;
                Result<TResult, TResponseError> result;
                try
                {
                    result = func((T)connection, requestMessage.@params, token);
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
        private static MethodInfo GetFactoryForRequest(Type declaringType, Type paramsType, Type resultType, Type responseErrorType)
        {
            return method_ForRequest4.MakeGenericMethod(declaringType, paramsType, resultType, responseErrorType);
        }

        private static readonly MethodInfo method_ForRequest3 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest3))!;
        private static RequestHandlerDelegate ForRequest3<T, TResult, TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new() where T : Connection
        {
            Func<T, CancellationToken, Result<TResult, TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<T, CancellationToken, Result<TResult, TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<T, Result<TResult, TResponseError>>>();
                func = (connection, token) => methodDelegate(connection);
            }

            return (request, connection, token) =>
            {
                var voidRequest = (VoidRequestMessage)request;
                Result<TResult, TResponseError> result;
                try
                {
                    result = func((T)connection, token);
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
        private static MethodInfo GetFactoryForRequest(Type declaringType, Type resultType, Type responseErrorType)
        {
            return method_ForRequest3.MakeGenericMethod(declaringType, resultType, responseErrorType);
        }

        private static readonly MethodInfo method_ForRequest2 = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForRequest2))!;
        private static RequestHandlerDelegate ForRequest2<T, TResponseError>(MethodInfo method, bool hasToken) where TResponseError : ResponseError, new() where T : Connection
        {
            Func<T, CancellationToken, VoidResult<TResponseError>> func;
            if (hasToken)
            {
                func = method.CreateDelegate<Func<T, CancellationToken, VoidResult<TResponseError>>>();
            }
            else
            {
                var methodDelegate = method.CreateDelegate<Func<T, VoidResult<TResponseError>>>();
                func = (connection, token) => methodDelegate(connection);
            }

            return (request, connection, token) =>
            {
                var voidRequest = (VoidRequestMessage)request;
                VoidResult<TResponseError> result;
                try
                {
                    result = func((T)connection, token);
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
        private static MethodInfo GetFactoryForRequest(Type declaringType, Type responseErrorType)
        {
            return method_ForRequest2.MakeGenericMethod(declaringType, responseErrorType);
        }

        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForRequest2(MethodInfo, Type, Type)")]
        private static MethodInfo GetFactoryForRequest(MethodInfo method, out bool hasToken)
        {
            if (!TryGetParameterType(method, out var parameterType, out hasToken)) throw new ArgumentException($"签名不匹配: {method.Name}");
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
            if (resultType == null) return GetFactoryForRequest(method.DeclaringType!, responseErrorType);
            else if (parameterType == null) return GetFactoryForRequest(method.DeclaringType!, resultType, responseErrorType);
            else return GetFactoryForRequest(method.DeclaringType!, parameterType, resultType, responseErrorType);
        }
        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForRequest2(MethodInfo, Type, Type)")]
        internal static RequestHandlerDelegate CreateRequestHandlerDelegate(MethodInfo method)
        {
            var factory = GetFactoryForRequest(method, out var hasToken);
            return (RequestHandlerDelegate)factory.Invoke(null, [method, hasToken])!;
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
        private static NotificationHandlerDelegate ForNotification2<T, TParams>(MethodInfo method) where T : Connection
        {
            var action = method.CreateDelegate<Action<T, TParams?>>();

            return (notification, connection) =>
            {
                var notificationMessage = (NotificationMessage<TParams>)notification;
                try
                {
                    action((T)connection, notificationMessage.@params);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            };
        }
        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForNotification(Type declaringType, Type paramsType)
        {
            return method_ForNotification2.MakeGenericMethod(declaringType, paramsType);
        }
        private static readonly MethodInfo method_ForNotification = typeof(Reflector).GetTypeInfo().GetDeclaredMethod(nameof(ForNotification))!;
        private static NotificationHandlerDelegate ForNotification<T>(MethodInfo method) where T : Connection
        {
            var action = method.CreateDelegate<Action<T>>();

            return (notification, connection) =>
            {
                try
                {
                    action((T)connection);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            };
        }

        [RequiresDynamicCode("Calls System.Reflection.MethodInfo.MakeGenericMethod(params Type[])")]
        private static MethodInfo GetFactoryForNotification(Type declaringType)
        {
            return method_ForNotification.MakeGenericMethod(declaringType);
        }

        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForNotification(Type, Type)")]
        private static MethodInfo GetFactoryForNotification(MethodInfo method)
        {
            if (method.ReturnType != typeof(void))
            {
                throw new ArgumentException($"signature mismatch: {method.Name}");
            }
            if (!TryGetParameterType(method, out var parameterType, out _))
            {
                throw new ArgumentException($"signature mismatch: {method.Name}");
            }
            if (parameterType != null) return GetFactoryForNotification(method.DeclaringType!, parameterType);
            else return GetFactoryForNotification(method.DeclaringType!);
        }
        [RequiresDynamicCode("Calls LanguageServer.Reflector.GetFactoryForNotification2(MethodInfo, Type, Type)")]
        internal static NotificationHandlerDelegate CreateNotificationHandlerDelegate(MethodInfo method)
        {
            return (NotificationHandlerDelegate)GetFactoryForNotification(method).Invoke(null, [method])!;
        }
        #endregion

        [SuppressMessage("Trimming", "IL2067:Target parameter argument does not satisfy 'DynamicallyAccessedMembersAttribute' in call to target method. The parameter of method does not have matching annotations.", Justification = "<挂起>")]
        [SuppressMessage("Trimming", "IL2072:Target parameter argument does not satisfy 'DynamicallyAccessedMembersAttribute' in call to target method. The return value of the source method does not have matching annotations.", Justification = "<挂起>")]
        internal static ResponseMessageBase CreateErrorResponse(Type responseType, NumberOrString id, string errorMessage)
        {
            var res = Activator.CreateInstance(responseType) as ResponseMessageBase;
            res!.id = id;
            var prop = responseType.GetRuntimeProperty("error");
            var err = Activator.CreateInstance(prop!.PropertyType) as ResponseError;
            err!.code = ErrorCodes.InternalError;
            err.message = errorMessage;
            prop.SetValue(res, err);
            return res;
        }
    }
}
