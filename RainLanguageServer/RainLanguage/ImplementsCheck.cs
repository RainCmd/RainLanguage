﻿namespace RainLanguageServer.RainLanguage
{
    internal partial class CompilingLibrary
    {
        private static void CollectInherits(ASTManager manager, List<Type> types, HashSet<CompilingInterface> interfaces)
        {
            foreach (var type in types)
                if (manager.GetSourceDeclaration(type) is CompilingInterface compiling)
                {
                    if (interfaces.Add(compiling))
                        CollectInherits(manager, compiling.inherits, interfaces);
                }
        }
        private static CompilingVirtualFunction? FindImplement(CompilingClass compiling, CompilingAbstractFunction function, TextRange? self)
        {
            foreach (var member in compiling.functions)
                if (member.declaration.visibility.ContainAny(Visibility.Public | Visibility.Protected))
                    if (member.name.ToString() == function.name.ToString() && member.declaration.signature == function.declaration.signature)
                    {
                        function.implements.Add(member);
                        if (function.returns != member.returns)
                        {
                            if (self == null)
                            {
                                var msg = new CompileMessage(member.name, CErrorLevel.Error, "函数返回值类型与接口函数不一致");
                                msg.related.Add(function.name);
                                member.file?.space.collector.Add(msg);
                            }
                            else
                            {
                                var msg = new CompileMessage(self, CErrorLevel.Error, "父类实现函数返回值类型与接口函数不一致");
                                msg.related.Add(member.name);
                                msg.related.Add(function.name);
                                member.file?.space.collector.Add(msg);
                            }
                        }
                        return member;
                    }
            return null;
        }
        public void ImplementsCheck(ASTManager manager)
        {
            var classSet = new HashSet<CompilingClass>();
            var interfaceSet = new HashSet<CompilingInterface>();
            foreach (var compiling in classes)
            {
                foreach (var function in compiling.functions)
                {
                    for (var index = compiling.parent; index.Vaild; index = manager.GetParent(index))
                    {
                        if (manager.GetSourceDeclaration(index) is CompilingClass parent && classSet.Add(parent))
                            foreach (var member in parent.functions)
                                if (member.name.ToString() == function.name && member.declaration.signature == function.declaration.signature)
                                {
                                    if (member.declaration.visibility.ContainAny(Visibility.Public | Visibility.Protected) && member.returns != function.returns)
                                    {
                                        var msg = new CompileMessage(function.name, CErrorLevel.Error, "重载的函数返回值不一致");
                                        msg.related.Add(member.name);
                                        function.file?.space.collector.Add(msg);
                                    }
                                    function.overrides.Add(member);
                                    member.implements.Add(function);
                                    goto label_next_member;
                                }
                    }
                label_next_member:
                    classSet.Clear();
                }
                CollectInherits(manager, compiling.inherits, interfaceSet);
                foreach (var inherit in interfaceSet)
                    foreach (var callable in inherit.callables)
                    {
                        var function = FindImplement(compiling, callable, null);
                        function?.overrides.Add(callable);
                        if (function == null)
                        {
                            for (var index = compiling.parent; index.Vaild; index = manager.GetParent(index))
                                if (manager.GetSourceDeclaration(index) is CompilingClass parent && classSet.Add(parent))
                                    if (FindImplement(parent, callable, compiling.name) != null) break;
                            classSet.Clear();
                        }
                    }
                interfaceSet.Clear();
            }
        }
    }
}
