import { ContinuedEvent, InitializedEvent, LoggingDebugSession, Scope, StoppedEvent, TerminatedEvent, Variable } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import * as RainDebug from './DebugConfigurationProvider'
import * as client from '../ClientHelper'
import { kernelStateViewProvider } from '../extension'

enum StepType {
	// 逐过程
	Over = 2,
	// 单步
	Into,
	// 跳出
	Out,
}
class SpaceNode{
	public parent: SpaceNode = null
	public children: SpaceNode[] = null
	public variables: VariableNode[] = null
	constructor(public frameId: number, public id: number, public name: string) { }
	public has(node: VariableNode) {
		for (let index = node.GetRoot().space; index; index = index.parent){
			if (index == this) {
				return true
			}
		}
		return false
	}
	public GetNames() {
		const result: string[] = []
		for (let index: SpaceNode = this; index.parent; index = index.parent) {
			result.push(index.name)	
		}
		return result.reverse()
	}
}
class VariableNode {
	public space: SpaceNode = null
	public parent: VariableNode = null
	public index: number
	public members: VariableNode[] = null
	constructor(public frameId: number, public id: number, public name: string, public type: string, public value: string) { }
	public GetRoot() {
		let index: VariableNode = this;
		while (index.parent) {
			index = index.parent
		}
		return index
	}
	public GetMemberIndices() {
		const indices: number[] = []
		for (let index: VariableNode = this; index.parent; index = index.parent){
			indices.push(index.index)
		}
		return indices.reverse()
	}
}
class FrameInfo {
	public id: number
	public locals: SpaceNode = null
	public globals: SpaceNode = null
	constructor(public thread: bigint, public index: number) {
		this.id = Number(thread) + index
	}
}
export class RainDebugSession extends LoggingDebugSession {
	private helper = new client.ClientHelper(RainDebug.client)
	constructor(protected configuration: RainDebug.RainDebugConfiguration) {
		super();
	}
	private GetRelativePath(value: string): string {
		return value.substring(this.configuration.projectPath.length).replace(/^[\\/]*/gi, "")
	}
	protected attachRequest(response: DebugProtocol.AttachResponse, args: DebugProtocol.AttachRequestArguments, request?: DebugProtocol.Request): void {
		console.log("attach")
		this.sendResponse(response)
	}
	protected launchRequest(response: DebugProtocol.LaunchResponse, args: DebugProtocol.LaunchRequestArguments, request?: DebugProtocol.Request): void {
		console.log("lanuch")
		this.sendResponse(response)
	}

	private InitStateView() {
		const writer = new client.Writer(client.Proto.RECV_Diagnose)
		writer.WriteUint(1)
		this.helper.Send(writer)
		kernelStateViewProvider.Show()
		kernelStateViewProvider.on("ChangeVisibility", visiable => {
			const writer = new client.Writer(client.Proto.RECV_Diagnose)
			if (visiable) {
				writer.WriteUint(1)
			} else {
				writer.WriteUint(0)
			}
			this.helper.Send(writer)
		})
	}
	
	private RegistDebuggerEvent() {
		this.helper.on(client.Proto.SEND_Attached, reader => {
			this.sendEvent(new InitializedEvent())
			this.InitStateView()
		}).on(client.Proto.SEND_OnBreak, reader => {
			let taskCount = reader.ReadInt()
			while (taskCount-- > 0) {
				this.sendEvent(new StoppedEvent("", Number(reader.ReadLong())))
			}
			const threadId = Number(reader.ReadLong())
			this.sendEvent(new StoppedEvent("命中断点", threadId));
		}).on(client.Proto.SEND_OnException, reader => {
			const msg = reader.ReadString()
			if (this.exitBreakCondition == null || this.exitBreakCondition.test(msg)) {
				let taskCount = reader.ReadInt()
				while (taskCount-- > 0) {
					this.sendEvent(new StoppedEvent("", Number(reader.ReadLong())))
				}
				const threadId = Number(reader.ReadLong())
				this.sendEvent(new StoppedEvent("exception", threadId, msg));
			} else {
				const writer = new client.Writer(client.Proto.RECV_Continue)
				writer.WriteBool(false)
				this.helper.Send(writer)
			}
		}).on(client.Proto.SEND_Message, reader => {
			const msg = reader.ReadString()
			console.log(msg)
		}).on(client.Proto.SEND_Diagnose, reader => {
			kernelStateViewProvider.RecvData({
				label: reader.ReadString(),
				task: reader.ReadInt(),
				string: reader.ReadInt(),
				entity: reader.ReadInt(),
				handle: reader.ReadInt(),
				heap: reader.ReadInt(),
			})
		})
	}

	protected initializeRequest(response: DebugProtocol.InitializeResponse, args: DebugProtocol.InitializeRequestArguments): void {
		RainDebug.client.on('close', () => {
			this.sendEvent(new TerminatedEvent())
		})
		this.RegistDebuggerEvent()
		
		response.body = {
			//调试适配器支持' configurationDone '请求。
			supportsConfigurationDoneRequest: true,
			//调试适配器支持函数断点。
			supportsFunctionBreakpoints: false,
			//调试适配器支持条件断点。
			supportsConditionalBreakpoints: false,
			//调试适配器支持在达到指定次数后中断执行的断点。
			supportsHitConditionalBreakpoints: false,
			//调试适配器支持(无副作用)“评估”数据请求徘徊。
			supportsEvaluateForHovers: true,
			//' setExceptionBreakpoints '请求的可用异常过滤器选项。
			exceptionBreakpointFilters: [
				{
					filter: '退出断点条件',
					label: "退出断点条件",
					description: `只有当退出信息包含输入的字符串时时才会引发断点`,
					default: true,
					supportsCondition: true,
					conditionDescription: `输入退出信息需要包含的字符串`
				}
			],
			//调试适配器支持通过' stepBack '和' reverseconcontinue '请求后退。
			supportsStepBack: false,
			//调试适配器支持将变量设置为值。
			supportsSetVariable: true,
			//调试适配器支持重新启动帧。
			supportsRestartFrame: false,
			//调试适配器支持' gotoTargets '请求。
			supportsGotoTargetsRequest: false,
			//调试适配器支持' stepInTargets '请求。
			supportsStepInTargetsRequest: false,
			//调试适配器支持' completion '请求。
			supportsCompletionsRequest: false,
			//在REPL中应该触发完成的一组字符。如果没有指定，UI应该假定为'。'字符。
			completionTriggerCharacters: [".", "["],
			//调试适配器支持' modules '请求。
			supportsModulesRequest: false,
			//由调试适配器公开的附加模块信息集。
			additionalModuleColumns: [],
			//调试适配器支持的校验和算法。
			supportedChecksumAlgorithms: [],
			//调试适配器支持“重启”请求。在这种情况下，客户端不应该通过终止和重新启动适配器来实现“重启”，而是通过调用“重启”请求来实现。
			supportsRestartRequest: false,
			//调试适配器支持' setExceptionBreakpoints '请求上的' exceptionOptions '。
			supportsExceptionOptions: false,
			//调试适配器在' stackTrace '、' variables '和' evaluate '请求上支持' format '属性。
			supportsValueFormattingOptions: false,
			//调试适配器支持' exceptionInfo '请求。
			supportsExceptionInfoRequest: true,
			//调试适配器支持' disconnect '请求上的' terminateDebuggee '属性。
			supportTerminateDebuggee: true,
			//调试适配器支持' disconnect '请求上的' suspendDebuggee '属性。
			supportSuspendDebuggee: true,
			//调试适配器支持堆栈部分的延迟加载，这需要同时支持' startFrame '和' levels '参数以及' stackTrace '请求的' totalFrames '结果。
			supportsDelayedStackTraceLoading: false,
			//调试适配器支持' loaddsources '请求。
			supportsLoadedSourcesRequest: false,
			//调试适配器通过解释' SourceBreakpoint '的' logMessage '属性来支持日志点。
			supportsLogPoints: false,
			//调试适配器支持' terminateThreads '请求。
			supportsTerminateThreadsRequest: false,
			//调试适配器支持' setExpression '请求。
			supportsSetExpression: false,
			//调试适配器支持“终止”请求。
			supportsTerminateRequest: true,
			//调试适配器支持数据断点。
			supportsDataBreakpoints: false,
			//调试适配器支持' readMemory '请求。
			supportsReadMemoryRequest: false,
			//调试适配器支持' writemmemory '请求。
			supportsWriteMemoryRequest: false,
			//调试适配器支持“反汇编”请求。
			supportsDisassembleRequest: false,
			//调试适配器支持“取消”请求。
			supportsCancelRequest: true,
			//调试适配器支持' breakpointLocations '请求。
			supportsBreakpointLocationsRequest: false,
			//调试适配器支持' evaluate '请求中的' clipboard '上下文值。
			supportsClipboardContext: false,
			//调试适配器支持步进请求的步进粒度(参数'粒度')。
			supportsSteppingGranularity: false,
			//调试适配器支持根据指令引用添加断点。
			supportsInstructionBreakpoints: false,
			//调试适配器支持' filterOptions '作为' setExceptionBreakpoints '请求的参数。
			supportsExceptionFilterOptions: true,
			//调试适配器在执行请求(' continue '， ' next '， ' stepIn '， ' stepOut '， ' reverseContinue '， ' stepBack ')上支持' singleThread '属性。
			supportsSingleThreadExecutionRequests: false,
			//调试适配器支持的断点模式，如“硬件”或“软件”。如果存在，客户端可能允许用户选择一种模式，并将其包含在其' setBreakpoints '请求中。
			//客户端可以将此数组中的第一个适用模式作为设置断点的手势中的“默认”模式。
			breakpointModes: []
		}
		
		const req = new client.Writer(client.Proto.RRECV_Initialized)
		req.WriteUint(114514)
		this.helper.Request(114514, req).then(res => {
			if (this.configuration.type == "雨言调试运行") {
				RainDebug.debuggedProcess.stdin.write('y')
				RainDebug.debuggedProcess.stdin.end();
			}
			this.sendResponse(response)
		}).catch(reason => {
			response.success = false;
			this.sendResponse(response)
		})
	}
	protected configurationDoneRequest(response: DebugProtocol.ConfigurationDoneResponse, args: DebugProtocol.ConfigurationDoneArguments, request?: DebugProtocol.Request): void {
		this.helper.Send(new client.Writer(client.Proto.RECV_Confirm))
		this.sendResponse(response)
	}
	protected disconnectRequest(response: DebugProtocol.DisconnectResponse, args: DebugProtocol.DisconnectArguments, request?: DebugProtocol.Request): void {
		RainDebug.client.destroy();
		if (this.configuration.request == 'launch' && RainDebug.debuggedProcess != null && !RainDebug.debuggedProcess.killed) {
			RainDebug.debuggedProcess.kill()
		}
		kernelStateViewProvider.removeAllListeners()
		this.sendResponse(response)
	}
	
	private breakpointMap = new Map<string, number[]>()
	protected setBreakPointsRequest(response: DebugProtocol.SetBreakpointsResponse, args: DebugProtocol.SetBreakpointsArguments, request?: DebugProtocol.Request): void {
		const path = this.GetRelativePath(args.source.path)
		let lines = this.breakpointMap.get(path)
		if (lines == undefined) {
			lines = []
		}
		const removed = lines.filter(v => !args.lines.includes(v))
		if (removed.length > 0) {
			const removeReq = new client.Writer(client.Proto.RECV_RemoveBreaks);
			removeReq.WriteString(path)
			removeReq.WriteUint(removed.length)
			removed.forEach(value => removeReq.WriteUint(value))
			this.helper.Send(removeReq)
		}
		const added = args.lines
		if (added.length > 0) {
			const req = new client.Writer(client.Proto.RRECV_AddBreadks)
			req.WriteUint(request.seq)
			req.WriteString(path)
			req.WriteUint(added.length)
			added.forEach(value => req.WriteUint(value))
			this.helper.Request(request.seq, req).then(res => {
				let cnt = res.ReadInt()
				let invalidLines: number[] = []
				while (cnt-- > 0) {
					invalidLines.push(res.ReadInt());
				}
				response.body = { breakpoints: [] }
				args.lines.forEach(v => {
					response.body.breakpoints.push({
						verified: !invalidLines.includes(v)
					})
				})
				this.breakpointMap.set(path, args.lines)
				this.sendResponse(response)
			}).catch(reason => {
				response.success = false
				this.sendResponse(response)
			})
		}
	}
	private exitBreakCondition: RegExp = null;
	protected setExceptionBreakPointsRequest(response: DebugProtocol.SetExceptionBreakpointsResponse, args: DebugProtocol.SetExceptionBreakpointsArguments, request?: DebugProtocol.Request): void {
		const option = args.filterOptions.find(value => value.filterId == "退出断点条件")
		if (option && option.condition && option.condition != "") {
			this.exitBreakCondition = new RegExp(option.condition)
		} else {
			this.exitBreakCondition = null
			response.success = false
		}
		this.sendResponse(response)
	}
	protected threadsRequest(response: DebugProtocol.ThreadsResponse, request?: DebugProtocol.Request): void {
		this.traceMap.clear()
		this.spaceMap.clear()
		this.variableMap.clear()
		let req = new client.Writer(client.Proto.RRECV_Tasks)
		req.WriteUint(request.seq)
		this.helper.Request(request.seq, req).then(res => {
			let cnt = res.ReadInt()
			response.body = { threads: [] }
			while (cnt-- > 0) {
				let id = Number(res.ReadLong())
				response.body.threads.push({
					id: id,
					name: "TaskID:" + id.toString()
				})
			}
			this.sendResponse(response);
		}).catch(reason => {
			response.success = false
			this.sendResponse(response);
		})
	}
	private traceMap = new Map<number, FrameInfo>()
	protected stackTraceRequest(response: DebugProtocol.StackTraceResponse, args: DebugProtocol.StackTraceArguments, request?: DebugProtocol.Request): void {
		const threadId = BigInt(args.threadId)
		const req = new client.Writer(client.Proto.RRECV_Task)
		req.WriteUint(request.seq)
		req.WriteLong(threadId)
		this.helper.Request(request.seq, req).then(res => {
			const stacks: DebugProtocol.StackFrame[] = []
			let cnt = res.ReadInt()
			for (let index = 0; index < cnt; index++) {
				const file = res.ReadString()
				const name = res.ReadString()
				const frameInfo = new FrameInfo(threadId, index + 1)
				this.traceMap.set(frameInfo.id, frameInfo)
				stacks.push({
					id: frameInfo.id,
					name: name,
					line: res.ReadInt(),
					source: {
						path: this.configuration.projectPath + "\\" + file
					},
					column: 0
				})
			}
			response.body = {
				stackFrames: stacks,
				totalFrames: stacks.length
			}
			this.sendResponse(response)
		}).catch(reason => {
			response.success = false
			this.sendResponse(response)
		})
	}

	private referenceIndex = 0
	private spaceMap = new Map<number, SpaceNode>()
	private variableMap = new Map<number, VariableNode>()
	private CreateSpaceNode(frameId: number, name: string) {
		const result = new SpaceNode(frameId, ++this.referenceIndex, name)
		this.spaceMap.set(result.id, result)
		return result
	}
	private CreateVariable(frameId: number,name: string, structured: boolean, type: string, value: string) {
		if (structured) {
			const result = new VariableNode(frameId, ++this.referenceIndex, name, type, value)
			this.variableMap.set(result.id, result)
			return result
		}
		return new VariableNode(frameId, 0, name, type, value)
	}
	protected scopesRequest(response: DebugProtocol.ScopesResponse, args: DebugProtocol.ScopesArguments, request?: DebugProtocol.Request): void {
		const locals = this.CreateSpaceNode(args.frameId, "局部变量")
		const globals = this.CreateSpaceNode(args.frameId, this.configuration.projectName)
		const trace = this.traceMap.get(args.frameId)
		if (trace) {
			trace.locals = locals
			trace.globals = globals
		}
		response.body = {
			scopes: [
				new Scope(locals.name, locals.id, false),
				new Scope(globals.name, globals.id, true)
			]
		}
		this.sendResponse(response)
	}

	private ReadVariableMembers(src: VariableNode, variables: DebugProtocol.Variable[], reader: client.Reader) {
		let count = reader.ReadInt()
		src.members = []
		while (count-- > 0) {
			const node = this.CreateVariable(src.frameId, reader.ReadString(), reader.ReadBool(), reader.ReadString(), reader.ReadString())
			node.parent = src
			node.index = src.members.length
			src.members.push(node)
			variables.push({
				name: node.name,
				value: node.value,
				variablesReference: node.id,
				type: node.type
			})
		}
	}
	private ReadSpaceVariables(space: SpaceNode, variables: DebugProtocol.Variable[], reader: client.Reader) {
		space.variables = []
		let count = reader.ReadInt()
		while (count-- > 0) {
			const node = this.CreateVariable(space.frameId, reader.ReadString(), reader.ReadBool(), reader.ReadString(), reader.ReadString())
			node.space = space
			space.variables.push(node)
			variables.push({
				name: node.name,
				value: node.value,
				variablesReference: node.id,
				type: node.type
			})
		}
	}
	protected variablesRequest(response: DebugProtocol.VariablesResponse, args: DebugProtocol.VariablesArguments, request?: DebugProtocol.Request): void {
		const variables: DebugProtocol.Variable[] = []
		const space = this.spaceMap.get(args.variablesReference)
		if (space) {
			const trace = this.traceMap.get(space.frameId)
			if (space == trace.locals) {
				const req = new client.Writer(client.Proto.RRECV_Trace)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				this.helper.Request(request.seq, req).then(res => {
					this.ReadSpaceVariables(space, variables, res)
					response.body = { variables: variables }
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			} else {
				const req = new client.Writer(client.Proto.RRECV_Space)
				req.WriteUint(request.seq)
				const names = space.GetNames()
				req.WriteUint(names.length)
				names.forEach(value => req.WriteString(value))
				this.helper.Request(request.seq, req).then(res => {
					space.children = []
					let count = res.ReadInt()
					while (count-- > 0) {
						const node = this.CreateSpaceNode(trace.id, res.ReadString())
						node.parent = space
						space.children.push(node)
						variables.push({
							name: node.name,
							variablesReference: node.id,
							value: ""
						})
					}
					this.ReadSpaceVariables(space, variables, res)
					response.body = { variables: variables }
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			}
			return
		}

		const variable = this.variableMap.get(args.variablesReference)
		if (variable) {
			const trace = this.traceMap.get(variable.frameId)
			if (trace.locals.has(variable)) {
				const req = new client.Writer(client.Proto.RRECV_Local)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				req.WriteString(variable.GetRoot().name)
				const indices = variable.GetMemberIndices()
				req.WriteUint(indices.length)
				indices.forEach(value => req.WriteUint(value))
				this.helper.Request(request.seq, req).then(res => {
					this.ReadVariableMembers(variable, variables, res)
					response.body = { variables: variables }
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			} else if(trace.globals.has(variable)) {
				const req = new client.Writer(client.Proto.RRECV_Global)
				req.WriteUint(request.seq)
				const names = variable.GetRoot().space.GetNames()
				req.WriteUint(names.length)
				names.forEach(value => req.WriteString(value))
				req.WriteString(variable.GetRoot().name)
				const indices = variable.GetMemberIndices()
				req.WriteUint(indices.length)
				indices.forEach(value => req.WriteUint(value))
				this.helper.Request(request.seq, req).then(res => {
					this.ReadVariableMembers(variable, variables, res)
					response.body = { variables: variables }
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			} else {
				const req = new client.Writer(client.Proto.RRECV_Hover)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				req.WriteString(this.hoverFile)
				req.WriteUint(this.hoverLine)
				req.WriteUint(this.hoverChar)
				const indices = variable.GetMemberIndices()
				req.WriteUint(indices.length)
				indices.forEach(value => req.WriteUint(value))
				this.helper.Request(request.seq, req).then(res => {
					this.ReadVariableMembers(variable, variables, res)
					response.body = { variables: variables }
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			}
			return
		}
		response.success = false
		this.sendResponse(response)
	}
	protected setVariableRequest(response: DebugProtocol.SetVariableResponse, args: DebugProtocol.SetVariableArguments, request?: DebugProtocol.Request): void {
		const space = this.spaceMap.get(args.variablesReference)
		if (space) {
			const trace = this.traceMap.get(space.frameId)
			const variable = space.variables.find(value => value.name == args.name);
			if (space == trace.locals) {
				const req = new client.Writer(client.Proto.RRECV_SetLocal)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				req.WriteString(variable.name)
				req.WriteUint(0)
				req.WriteString(args.value)
				this.helper.Request(request.seq, req).then(res => {
					response.body = {
						value: res.ReadString(),
						variablesReference: variable.id,
						type: variable.type,
					}
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false;
					this.sendResponse(response)
				})
			} else {
				const req = new client.Writer(client.Proto.RRECV_SetGlobal)
				req.WriteUint(request.seq)
				req.WriteUint(0)
				req.WriteString(variable.name)
				req.WriteUint(0)
				req.WriteString(args.value)
				this.helper.Request(request.seq, req).then(res => {
					response.body = {
						value: res.ReadString(),
						variablesReference: variable.id,
						type: variable.type,
					}
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false;
					this.sendResponse(response)
				})
			}
		} else {
			const parent = this.variableMap.get(args.variablesReference)
			const trace = this.traceMap.get(parent.frameId)
			if (trace.locals.has(parent)) {
				const req = new client.Writer(client.Proto.RRECV_SetLocal)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				req.WriteString(parent.GetRoot().name)
				const variable = parent.members.find(value => value.name == args.name)
				const indices = variable.GetMemberIndices()
				req.WriteUint(indices.length)
				indices.forEach(value => req.WriteUint(value))
				req.WriteString(args.value)
				this.helper.Request(request.seq, req).then(res => {
					response.body = {
						value: res.ReadString(),
						variablesReference: variable.id,
						type: variable.type,
					}
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false;
					this.sendResponse(response)
				})
			} else if (trace.globals.has(parent)) {
				const req = new client.Writer(client.Proto.RRECV_SetGlobal)
				req.WriteUint(request.seq)
				const names = parent.GetRoot().space.GetNames()
				req.WriteUint(names.length)
				names.forEach(value => req.WriteString(value))
				req.WriteString(parent.GetRoot().name)
				const variable = parent.members.find(value => value.name == args.name)
				const indices = variable.GetMemberIndices()
				req.WriteUint(indices.length)
				indices.forEach(value => req.WriteUint(value))
				req.WriteString(args.value)
				this.helper.Request(request.seq, req).then(res => {
					response.body = {
						value: res.ReadString(),
						variablesReference: variable.id,
						type: variable.type,
					}
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false;
					this.sendResponse(response)
				})
			}
			else {
				response.success = false
				this.sendResponse(response)
			}
		}
	}

	protected pauseRequest(response: DebugProtocol.PauseResponse, args: DebugProtocol.PauseArguments, request?: DebugProtocol.Request): void {
		this.helper.Send(new client.Writer(client.Proto.RECV_Pause))
		this.sendResponse(response)
	}
	protected continueRequest(response: DebugProtocol.ContinueResponse, args: DebugProtocol.ContinueArguments, request?: DebugProtocol.Request): void {
		this.sendEvent(new ContinuedEvent(args.threadId, true))
		const writer = new client.Writer(client.Proto.RECV_Continue)
		writer.WriteBool(true)
		this.helper.Send(writer)
		this.sendResponse(response)
	}
	protected nextRequest(response: DebugProtocol.NextResponse, args: DebugProtocol.NextArguments, request?: DebugProtocol.Request): void {
		this.sendEvent(new ContinuedEvent(args.threadId, true))
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Over)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	protected stepInRequest(response: DebugProtocol.StepInResponse, args: DebugProtocol.StepInArguments, request?: DebugProtocol.Request): void {
		this.sendEvent(new ContinuedEvent(args.threadId, true))
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Into)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	protected stepOutRequest(response: DebugProtocol.StepOutResponse, args: DebugProtocol.StepOutArguments, request?: DebugProtocol.Request): void {
		this.sendEvent(new ContinuedEvent(args.threadId, true))
		const req = new client.Writer(client.Proto.RECV_Step)
		req.WriteUint(StepType.Out)
		this.helper.Send(req)
		this.sendResponse(response)
	}
	private hoverFile: string
	private hoverLine: number
	private hoverChar: number
	protected evaluateRequest(response: DebugProtocol.EvaluateResponse, args: DebugProtocol.EvaluateArguments, request?: DebugProtocol.Request): void {
		if (args.context == 'hover') {
			let line: number
			let character: number
			let expr = args.expression
			let lastSpace = expr.lastIndexOf(' ')
			if (lastSpace > 0) {
				character = Number(expr.substring(lastSpace).trim())
				expr = expr.substring(0, lastSpace)
			}
			lastSpace = expr.lastIndexOf(' ')
			if (lastSpace > 0) {
				line = Number(expr.substring(lastSpace).trim())
				expr = expr.substring(0, lastSpace)
			}
			const trace = this.traceMap.get(args.frameId)
			if (line && character && trace) {
				const req = new client.Writer(client.Proto.RRECV_Eval)
				req.WriteUint(request.seq)
				req.WriteLong(trace.thread)
				req.WriteUint(trace.index)
				this.hoverFile = this.GetRelativePath(expr)
				this.hoverLine = Number(line)
				this.hoverChar = Number(character)
				req.WriteString(this.hoverFile)
				req.WriteUint(this.hoverLine)
				req.WriteUint(this.hoverChar)
				this.helper.Request(request.seq, req).then(res => {
					if (res.ReadBool()) {
						const node = this.CreateVariable(trace.id, "", res.ReadBool(), "", res.ReadString())
						response.body = {
							variablesReference: node.id,
							result: node.value
						}
					} else {
						response.success = false
					}
					this.sendResponse(response)
				}).catch(reason => {
					response.success = false
					this.sendResponse(response)
				})
			} else {
				response.success = false
				this.sendResponse(response)
			}
		}
	}
}