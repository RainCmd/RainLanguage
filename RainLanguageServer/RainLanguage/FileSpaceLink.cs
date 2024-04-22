using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        private Type GetType(Context context, ASTManager manager, FileType fileType)
        {
            Type result = default;
            if (context.TryFindDeclaration(manager, fileType.name, out var declarations, collector))
            {
                if (declarations.Count == 1)
                {
                    result = declarations[0].Declaration.GetDefineType();
                    if (!result.Vaild) collector.Add(fileType.name, CErrorLevel.Error, "无效的类型");
                }
                else
                {
                    var builder = new StringBuilder().AppendLine("类型不明确");
                    foreach (var item in declarations)
                        builder.AppendLine(item.GetFullName());
                    collector.Add(fileType.name, CErrorLevel.Error, builder.ToString());
                }
            }
            else collector.Add(fileType.name, CErrorLevel.Error, "声明未找到");
            return result;
        }
        private static void AddTypeCite(ASTManager manager, CompilingDeclaration source, Type type)
        {
            if (manager.GetSourceDeclaration(type) is CompilingDeclaration target)
                source.AddCite(target.file);
        }
        private static void AddTypeCites(ASTManager manager, CompilingDeclaration source, IList<Type> types)
        {
            foreach (var type in types) AddTypeCite(manager, source, type);
        }
        public void Link(ASTManager manager)
        {
            foreach (var child in children) child.Link(manager);
            var context = new Context(compiling, relies, default);
            foreach (var file in variables)
            {
                var type = GetType(context, manager, file.type);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Variable, compiling.GetChildName(file.name.ToString()), default);
                var variable = new CompilingVariable(file.name, declaration, compiling, file, file.isReadonly, type, file.expression, relies);
                manager.library.variables.Add(variable);
                compiling.AddDeclaration(variable);
                AddTypeCite(manager, variable, type);
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
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var function = new CompilingFunction(file.name, declaration, compiling, file, parameters, new Tuple(returnTypes), file.body, relies);
                manager.library.functions.Add(function);
                compiling.AddDeclaration(function);
                AddTypeCites(manager, function, parameterTypes);
                AddTypeCites(manager, function, returnTypes);
            }
            foreach (var file in enums)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingEnum = (CompilingEnum)declarations!.Find(value => value.file == file)!;

                foreach (var element in file.elements)
                {
                    var declaration = new Declaration(Type.LIBRARY_SELF, Visibility.Public, DeclarationCategory.EnumElement, compiling.GetMemberName(file.name.ToString(), element.name.ToString()), default);
                    var compilingEnumElement = new CompilingEnum.Element(element.name, declaration, element.expression, relies, element);
                    compilingEnumElement.AddCite(element);
                    compilingEnum.elements.Add(compilingEnumElement);
                }
            }
            foreach (var file in structs)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingStruct = (CompilingStruct)declarations!.Find(value => value.file == file)!;

                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.StructVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, compiling, variable, variable.isReadonly, type, variable.expression, relies);
                    compilingStruct.variables.Add(compilingVariable);
                    AddTypeCite(manager, compilingVariable, type);
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
                    var declaration = new Declaration(Type.LIBRARY_SELF, function.visibility, DeclarationCategory.StructFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, compiling, function, parameters, new Tuple(returnTypes), function.body, relies);
                    compilingStruct.functions.Add(compilingFunction);
                    AddTypeCites(manager, compilingFunction, parameterTypes);
                    AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach (var file in interfaces)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingInterface = (CompilingInterface)declarations!.Find(value => value.file == file)!;

                foreach (var fileType in file.inherits)
                {
                    var type = GetType(context, manager, fileType);
                    compilingInterface.inherits.Add(type);
                    AddTypeCite(manager, compilingInterface, type);
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
                    var declaration = new Declaration(Type.LIBRARY_SELF, function.visibility, DeclarationCategory.InterfaceFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingCallable(function.name, declaration, compiling, function, parameters, new Tuple(returnTypes));
                    compilingInterface.callables.Add(compilingFunction);
                    AddTypeCites(manager, compilingFunction, parameterTypes);
                    AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach (var file in classes)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingClass = (CompilingClass)declarations!.Find(value => value.file == file)!;

                for (int i = 0; i < file.inherits.Count; i++)
                {
                    var type = GetType(context, manager, file.inherits[i]);
                    if (i > 0) compilingClass.inherits.Add(type);
                    else compilingClass.parent = type;
                    AddTypeCite(manager, compilingClass, type);
                }
                foreach (var variable in file.variables)
                {
                    var type = GetType(context, manager, variable.type);
                    var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.ClassVariable, compiling.GetMemberName(file.name.ToString(), variable.name.ToString()), default);
                    var compilingVariable = new CompilingVariable(variable.name, declaration, compiling, variable, variable.isReadonly, type, variable.expression, relies);
                    compilingClass.variables.Add(compilingVariable);
                    AddTypeCite(manager, compilingVariable, type);
                }
                foreach (var function in file.constructors)
                {
                    var parameters = new List<CompilingCallable.Parameter>();
                    foreach (var parameter in function.parameters)
                        parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                    var parameterTypes = new List<Type>();
                    foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                    var declaration = new Declaration(Type.LIBRARY_SELF, function.visibility, DeclarationCategory.Constructor, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, compiling, function, parameters, new Tuple([]), function.body, relies);
                    compilingClass.constructors.Add(compilingFunction);
                    AddTypeCites(manager, compilingFunction, parameterTypes);
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
                    var declaration = new Declaration(Type.LIBRARY_SELF, function.visibility, DeclarationCategory.ClassFunction, compiling.GetMemberName(file.name.ToString(), function.name.ToString()), new Tuple(parameterTypes));
                    var compilingFunction = new CompilingFunction(function.name, declaration, compiling, function, parameters, new Tuple(returnTypes), function.body, relies);
                    compilingClass.functions.Add(compilingFunction);
                    AddTypeCites(manager, compilingFunction, parameterTypes);
                    AddTypeCites(manager, compilingFunction, returnTypes);
                }
            }
            foreach(var file in delegates)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingDelegate = (CompilingDelegate)declarations!.Find(value => value.file == file)!;

                var name = compilingDelegate.declaration.name;
                var parameters = new List<CompilingCallable.Parameter>();
                foreach (var parameter in file.parameters)
                    parameters.Add(new CompilingCallable.Parameter(parameter.name, GetType(context, manager, parameter.type)));
                var parameterTypes = new List<Type>();
                foreach (var parameter in parameters) parameterTypes.Add(parameter.type);
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Delegate, name, new Tuple(parameterTypes));

                declarations.Remove(compilingDelegate);
                manager.library.delegates.Remove(compilingDelegate);

                compilingDelegate = new CompilingDelegate(file.name, declaration, compiling, file, parameters, new Tuple(returnTypes));
                AddTypeCites(manager, compilingDelegate, parameterTypes);
                AddTypeCites(manager, compilingDelegate, returnTypes);

                declarations.Add(compilingDelegate);
                manager.library.delegates.Add(compilingDelegate);
            }
            foreach(var file in tasks)
            {
                compiling.declarations.TryGetValue(file.name.ToString(), out var declarations);
                var compilingTask = (CompilingTask)declarations!.Find(value => value.file == file)!;

                var name = compilingTask.declaration.name;
                var returnTypes = new List<Type>();
                foreach (var type in file.returns)
                    returnTypes.Add(GetType(context, manager, type));
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Task, name, default);

                declarations.Remove(compilingTask);
                manager.library.tasks.Remove(compilingTask);

                compilingTask = new CompilingTask(file.name, declaration, compiling, file, new Tuple(returnTypes));
                AddTypeCites(manager, compilingTask, returnTypes);

                declarations.Add(compilingTask);
                manager.library.tasks.Add(compilingTask);
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
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Function, compiling.GetChildName(file.name.ToString()), new Tuple(parameterTypes));
                var native = new CompilingNative(file.name, declaration, compiling, file, parameters, new Tuple(returnTypes));
                manager.library.natives.Add(native);
                compiling.AddDeclaration(native);
                AddTypeCites(manager, native, parameterTypes);
                AddTypeCites(manager, native, returnTypes);
            }
        }
    }
}
