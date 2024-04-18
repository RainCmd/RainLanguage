﻿using System.Diagnostics.CodeAnalysis;
using System.Reflection;

namespace LanguageServer
{
    internal class HandlerProvider
    {
        internal static void AddHandlers<T>(RequestHandlerCollection requestHandlers, NotificationHandlerCollection notificationHandlers) where T : Connection
        {
            foreach (var method in typeof(T).GetRuntimeMethods())
            {
                var rpcMethod = method.GetCustomAttribute<JsonRpcMethodAttribute>()?.Method;
                if (rpcMethod != null)
                {
                    if (Reflector.IsRequestHandler(method))
                    {
                        requestHandlers.AddRequestHandler(new RequestHandler(
                            rpcMethod,
                            Reflector.GetRequestType(method),
                            Reflector.GetResponseType(method),
                            Reflector.CreateRequestHandlerDelegate(method)
                            ));
                    }
                    else if (Reflector.IsNotificationHandler(method))
                    {
                        notificationHandlers.AddNotificationHandler(new NotificationHandler(
                            rpcMethod,
                            Reflector.GetNotificationType(method),
                            Reflector.CreateNotificationHandlerDelegate(method)
                            ));
                    }
                }
            }
        }
    }
}
