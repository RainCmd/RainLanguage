using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        private Type GetType(Context context, ASTManager manager, FileType fileType)//todo 这里要把错误消息收集器传进来
        {
            Type result = default;
            if (context.TryFindDeclaration(manager, fileType.name, out var declarations, collector))
            {
                if (declarations.Count == 1)
                {
                    result = declarations[0].declaration.GetDefineType();
                    if (!result.Vaild) collector.Add(fileType.name, CErrorLevel.Error, "无效的类型");
                }
                else
                {
                    var msg = new CompileMessage(fileType.name, CErrorLevel.Error, "类型不明确");
                    foreach (var item in declarations)
                        msg.related.Add(new RelatedInfo(item.name, item.GetFullName()));
                    collector.Add(msg);
                }
            }
            else collector.Add(fileType.name, CErrorLevel.Error, "声明未找到");
            return new Type(result.library!, result.code, result.name!, fileType.dimension);
        }
        private static void AddTypeCite(ASTManager manager, CompilingDeclaration compiling, Type type)
        {
            if (manager.GetSourceDeclaration(type) is CompilingDeclaration target && target.file != null)
                compiling.AddCite(target.file);
        }
        private static void AddTypeCites(ASTManager manager, CompilingDeclaration source, IList<Type> types)
        {
            foreach (var type in types) AddTypeCite(manager, source, type);
        }
        public void Link(ASTManager manager, CompilingLibrary library, bool cite)
        {
            foreach (var child in children) child.Link(manager, library, cite);
            var context = new Context(compiling, relies, default);
            foreach (var file in variables)
            {
                var type = GetType(context, manager, file.type);
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Variable, compiling.GetChildName(file.name.ToString()), default);
                var variable = new CompilingVariable(file.name, declaration, file.attributes, compiling, cite ? file : null, file.isReadonly, type, file.expression, relies);
                library.variables.Add(variable);
                compiling.AddDeclaration(variable);
                if (cite) AddTypeCite(manager, variable, type);
            }
            foreach (var file in functions)
            {
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var function = new CompilingFunction(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes), file.body, relies);
                library.functions.Add(function);
                compiling.AddDeclaration(function);
                if (cite) AddTypeCites(manager, function, parameterTypes);
                if (cite) AddTypeCites(manager, function, returnTypes);
            }
            foreach (var file in enums)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingEnum = (CompilingEnum)declarations!.Find(value => value.name == file.name)!;

                foreach (var element in file.elements)
                {
                    var declaration = new Declaration(library.name, Visibility.Public, DeclarationCategory.EnumElement, compiling.GetMemberName(file.name.ToString(), element.name.ToString()), default);
                    var compilingEnumElement = new CompilingEnum.Element(element.name, declaration, element.expression, relies, cite ? element : null);
                    compilingEnum.elements.Add(compilingEnumElement);
                }
            }
            foreach (var file in structs)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingStruct = (CompilingStruct)declarations!.Find(value => value.name == file.name)!;

                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.StructVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, variable.attributes, compiling, cite ? variable : null, variable.isReadonly, type, variable.expression, relies);
                    compilingStruct.variables.Add(compilingVariable);
                    if (cite) AddTypeCite(manager, compilingVariable, type);
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.StructFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes), function.body, relies);
                    compilingStruct.functions.Add(compilingFunction);
                    if (cite) AddTypeCites(manager, compilingFunction, parameterTypes);
                    if (cite) AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach (var file in interfaces)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingInterface = (CompilingInterface)declarations!.Find(value => value.name == file.name)!;

                foreach (var fileType in file.inherits)
                {
                    var type = GetType(context, manager, fileType);
                    if (type.dimension > 0) file.collector.Add(fileType.name, CErrorLevel.Error, "不能继承数组");
                    else if (type.code == TypeCode.Interface || type == Type.HANDLE) compilingInterface.inherits.Add(type);
                    else file.collector.Add(fileType.name, CErrorLevel.Error, "必须是接口");
                    if (cite) AddTypeCite(manager, compilingInterface, type);
                    if (manager.GetSourceDeclaration(type) is CompilingInterface inherit)
                        compilingInterface.implements.Add(inherit);
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.InterfaceFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingAbstractFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes));
                    compilingInterface.callables.Add(compilingFunction);
                    if (cite) AddTypeCites(manager, compilingFunction, parameterTypes);
                    if (cite) AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach (var file in classes)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingClass = (CompilingClass)declarations!.Find(value => value.name == file.name)!;

                for (int i = 0; i < file.inherits.Count; i++)
                {
                    var type = GetType(context, manager, file.inherits[i]);
                    if (type.dimension > 0) file.collector.Add(file.inherits[i].name, CErrorLevel.Error, "不能继承数组");
                    else if (type.code == TypeCode.Interface) compilingClass.inherits.Add(type);
                    else if (i > 0) file.collector.Add(file.inherits[i].name, CErrorLevel.Error, "必须是接口");
                    else if (type.code == TypeCode.Handle) compilingClass.parent = type;
                    else file.collector.Add(file.inherits[i].name, CErrorLevel.Error, "不能继承该类型");
                    if (cite) AddTypeCite(manager, compilingClass, type);
                    var inherit = manager.GetSourceDeclaration(type);
                    if (inherit is CompilingInterface @interface) @interface.implements.Add(compilingClass);
                    else if (inherit is CompilingClass @class) @class.implements.Add(compilingClass);
                }
                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.ClassVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, variable.attributes, compiling, cite ? variable : null, variable.isReadonly, type, variable.expression, relies);
                    compilingClass.variables.Add(compilingVariable);
                    if (cite) AddTypeCite(manager, compilingVariable, type);
                }
                foreach (var function in file.constructors)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.Constructor, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple([]), function.body, relies);
                    compilingClass.constructors.Add(compilingFunction);
                    if (cite) AddTypeCites(manager, compilingFunction, parameterTypes);
                }
                foreach (var function in file.functions)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var returnTypes = new List<Type>();
                    foreach (var type in function.returns)
                        returnTypes.Add(GetType(context, manager, type));
                    var declaration = new Declaration(library.name, function.visibility, DeclarationCategory.ClassFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingVirtualFunction(function.name, declaration, function.attributes, compiling, cite ? function : null, parameters, new Tuple(returnTypes), function.body, relies);
                    compilingClass.functions.Add(compilingFunction);
                    if (cite) AddTypeCites(manager, compilingFunction, parameterTypes);
                    if (cite) AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach (var file in delegates)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingDelegate = (CompilingDelegate)declarations!.Find(value => value.name == file.name)!;

                var name = compilingDelegate.declaration.name;
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Delegate, name, new Tuple(parameterTypes));

                declarations.Remove(compilingDelegate);
                library.delegates.Remove(compilingDelegate);

                compilingDelegate = new CompilingDelegate(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes));
                if (cite) AddTypeCites(manager, compilingDelegate, parameterTypes);
                if (cite) AddTypeCites(manager, compilingDelegate, returnTypes);

                declarations.Add(compilingDelegate);
                library.delegates.Add(compilingDelegate);
            }
            foreach (var file in tasks)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingTask = (CompilingTask)declarations!.Find(value => value.name == file.name)!;

                var name = compilingTask.declaration.name;
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Task, name, default);

                declarations.Remove(compilingTask);
                library.tasks.Remove(compilingTask);

                compilingTask = new CompilingTask(file.name, declaration, file.attributes, compiling, cite ? file : null, new Tuple(returnTypes));
                if (cite) AddTypeCites(manager, compilingTask, returnTypes);

                declarations.Add(compilingTask);
                library.tasks.Add(compilingTask);
            }
            foreach (var file in natives)
            {
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var native = new CompilingNative(file.name, declaration, file.attributes, compiling, cite ? file : null, parameters, new Tuple(returnTypes));
                library.natives.Add(native);
                compiling.AddDeclaration(native);
                if (cite) AddTypeCites(manager, native, parameterTypes);
                if (cite) AddTypeCites(manager, native, returnTypes);
            }
        }
    }
}
