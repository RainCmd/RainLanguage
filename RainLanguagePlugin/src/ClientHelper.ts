
import * as net from 'net'
import { TextDecoder, TextEncoder } from 'util';

export enum Proto {
	//	uint32	requestId
	RRECV_Initialized,
	//	uint32	requestId
	RSEND_Initialized,
	SEND_Attached,
    RECV_Confirm,

	//	uint32	requestId
	//	string	file
	//	uint32	lineCount
	//		uint32	line
	RRECV_AddBreadks,
	//	发送的都是添加失败的断点
	//	uint32	requestId
	//	uint32	lineCount
	//		uint32	line
	RSEND_AddBreadks,
	//	string	file
	//	uint32	lineCount
	//		uint32	line
	RECV_RemoveBreaks,
	RECV_ClearBreaks,

	RECV_Pause,
	//	bool	igonreStep
	RECV_Continue,
	//	uint32	StepType
	RECV_Step,

	//	string	message
	//	uint32	taskCount
	//		uint64	taskId
	//	uint64	currentTask
	SEND_OnException,
	//	uint32	taskCount
	//		uint64	taskId
	//	uint64	currentTask
	SEND_OnBreak,

	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	RRECV_Space,
	//	uint32	requestId
	//	uint32	spaceCount
	//		string	spaceName
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Space,

	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Global,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Global,
	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RRECV_SetGlobal,
	//	uint32	requestId
	//	string	value
	RSEND_SetGlobal,

	//	uint32	requestId
	RRECV_Tasks,
	//	uint32	requestId
	//	uint32	taskCount
	//		uint64	taskId
	RSEND_Tasks,
	//	uint32	requestId
	//	uint64	taskId
	RRECV_Task,
	//	uint32	requestId
	//	uint32	traceCount
	//		string	file
	//		uint32	line
	RSEND_Task,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	RRECV_Trace,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Trace,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Local,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Local,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RRECV_SetLocal,
	//	uint32	requestId
	//	string	value
	RSEND_SetLocal,

	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	file
	//	uint32	line
	//	uint32	character
	RRECV_Eval,
	//	uint32	requestId
	//	bool	hasResult
	//		bool	structured
	//		string	value
	RSEND_Eval,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	file
	//	uint32	line
	//	uint32	character
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Hover,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Hover,

	//	uint32	frame
    RECV_Diagnose,
    //  string  label
	//	uint32	task
	//	uint32	string
	//	uint32	entity
	//	uint32	handle
	//	uint32	heap
	SEND_Diagnose,

	//	string msg
	SEND_Message,
}
const MAGIC_NUMBER: number = 0x4e494152;
export class Reader {
    private buffer: Buffer
    private position: number = 8
    private valid = false
    private mark: number = 0
    constructor(source: Buffer, size: number) {
        this.buffer = Buffer.alloc(size)
        source.copy(this.buffer, 0, 0, size);
        let number = this.buffer.readUint32LE(4)
        for (let index = 8; index < size; index++) {
            number -= this.buffer.readUint8(index)
        }
        this.valid = number == MAGIC_NUMBER
    }
    public IsValid() { return this.valid; }
    public Mark() {
        this.mark = this.position
    }
    public Regain() {
        this.position = this.mark
    }
    public ReadBool(): boolean{
        return this.buffer.readUint8(this.position++) != 0
    }
    public ReadProto(): Proto {
        const result: Proto = this.buffer.readUint32LE(this.position);
        this.position += 4;
        return result
    }
    public ReadInt(): number{
        const result = this.buffer.readUint32LE(this.position)
        this.position += 4
        return result
    }
    public ReadLong(): bigint{
        const result = this.buffer.readBigUInt64LE(this.position)
        this.position += 8
        return result
    }
    public ReadString(): string {
        const length = this.ReadInt()
        const result = new TextDecoder().decode(this.buffer.subarray(this.position, this.position + length))
        this.position += length
        return result
    }
}
enum ValueType{
    BOOL,
    UINT,
    LONG,
    BUFFER
}
class WriterValue {
    constructor(public type: ValueType, public value: any) { }
}
export class Writer {
    values: WriterValue[] = []
    size: number = 0
    constructor(value: Proto) {
        this.WriteUint(value)
    }
    public WriteBool(value: boolean) {
        this.values.push({
            type: ValueType.BOOL,
            value: value ? 1 : 0
        })
        this.size++
    }
    public WriteUint(value: number) {
        this.values.push({
            type: ValueType.UINT,
            value: value
        })
        this.size += 4
    }
    public WriteLong(value: bigint) {
        this.values.push({
            type: ValueType.LONG,
            value: value
        })
        this.size += 8
    }
    public WriteString(value: string) {
        let buf = Buffer.from(new TextEncoder().encode(value))
        Buffer.alloc
        this.WriteUint(buf.length)
        this.values.push({
            type: ValueType.BUFFER,
            value: buf
        })
        this.size += buf.length
    }
    public GetBuffer(): Buffer {
        let result = Buffer.alloc(this.size + 8);
        result.writeUint32LE(this.size + 8)
        let index = 8
        this.values.forEach(element => {
            switch (element.type) {
                case ValueType.BOOL:
                    result.writeUint8(element.value, index++)
                    break;
                case ValueType.UINT:
                    result.writeUint32LE(element.value, index)
                    index += 4
                    break
                case ValueType.LONG:
                    result.writeBigUint64LE(element.value, index)
                    index += 8
                    break
                case ValueType.BUFFER:
                    let buf = element.value as Buffer
                    buf.copy(result, index, 0, buf.length)
                    index += buf.length
                    break
                default:
                    break;
            }
        });
        let magic = MAGIC_NUMBER
        for (let index = 8; index < result.length; index++) {
            magic += result.readUint8(index)
        }
        result.writeUint32LE(magic, 4)
        return result
    }
}
class ClinetRequest {
    constructor(public reslove: (reader: Reader) => void, public reject: () => void) { }
}
export class ClientHelper {
    private readBuffer: Buffer = Buffer.alloc(1024)
    private readBuffSize: number = 0;
    private requestMap = new Map<number, ClinetRequest>()
    private eventMap = new Map<Proto, ((reader: Reader) => void)[]>()
    constructor(private client: net.Socket) {
        client.on('data', buffer => {
            if (this.readBuffSize + buffer.length > this.readBuffer.length) {
                let newLen = this.readBuffer.length
                while (newLen < this.readBuffSize + buffer.length) {
                    newLen *= 1.5
                }
                let newBuf = Buffer.alloc(newLen)
                this.readBuffer.copy(newBuf, 0, 0, this.readBuffSize);
                this.readBuffer = newBuf
            }
            buffer.copy(this.readBuffer, this.readBuffSize, 0, buffer.length)
            this.readBuffSize += buffer.length
            
            while (this.readBuffSize >= 4) {
                const size = this.readBuffer.readInt32LE()
                if (size <= this.readBuffSize) {
                    const reader = new Reader(this.readBuffer, size)
                    this.readBuffer.copy(this.readBuffer, 0, size, this.readBuffSize)
                    this.readBuffSize -= size
                    if (reader.IsValid()) {
                        this.OnRecv(reader)
                    }
                } else {
                    break;
                }
            }
        })
    }
    private OnRecv(reader: Reader) {
        switch (reader.ReadProto()) {
            case Proto.RRECV_Initialized: break;
            case Proto.RSEND_Initialized:
                this.RecvRequest(reader)
                break;
            case Proto.SEND_Attached:
                this.RecvEvent(Proto.SEND_Attached, reader)
                break;
            case Proto.RECV_Confirm: break;
            case Proto.RRECV_AddBreadks: break;
            case Proto.RSEND_AddBreadks:
                this.RecvRequest(reader)
                break;
            case Proto.RECV_RemoveBreaks: break;
            case Proto.RECV_ClearBreaks: break;
            case Proto.RECV_Pause: break;
            case Proto.RECV_Continue: break;
            case Proto.RECV_Step: break;
            case Proto.SEND_OnException:
                this.RecvEvent(Proto.SEND_OnException, reader)
                break;
            case Proto.SEND_OnBreak:
                this.RecvEvent(Proto.SEND_OnBreak, reader)
                break;
            case Proto.RRECV_Space: break;
            case Proto.RSEND_Space:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Global: break;
            case Proto.RSEND_Global:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_SetGlobal: break;
            case Proto.RSEND_SetGlobal:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Tasks: break;
            case Proto.RSEND_Tasks:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Task: break;
            case Proto.RSEND_Task:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Trace: break;
            case Proto.RSEND_Trace:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Local: break;
            case Proto.RSEND_Local:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_SetLocal: break;
            case Proto.RSEND_SetLocal:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Eval: break;
            case Proto.RSEND_Eval:
                this.RecvRequest(reader)
                break;
            case Proto.RRECV_Hover: break;
            case Proto.RSEND_Hover:
                this.RecvRequest(reader)
                break;
            case Proto.RECV_Diagnose: break;
            case Proto.SEND_Diagnose:
                this.RecvEvent(Proto.SEND_Diagnose, reader)
                break;
            case Proto.SEND_Message:
                this.RecvEvent(Proto.SEND_Message, reader)
                break;
            default:
                break;
        }
    }
    private RecvRequest(reader:Reader) {
        let id = reader.ReadInt()
        let request = this.requestMap.get(id)
        if (request) {
            request.reslove(reader)
            this.requestMap.delete(id)
        }
    }
    public async Request(requestId: number, writer: Writer): Promise<Reader> {
        return new Promise((reslove, reject) => {
            this.requestMap.set(requestId, new ClinetRequest(reslove, reject))
            setTimeout(() => {
                if (this.requestMap.delete(requestId)) {
                    reject()
                }
            }, 2000)
            this.Send(writer)
        })
    }
    public Send(writer: Writer) {
        this.client.write(writer.GetBuffer())
    }
    private RecvEvent(proto: Proto, reader: Reader) {
        let ls = this.eventMap.get(proto)
        reader.Mark()
        if (ls) {
            ls.forEach(element => {
                reader.Regain()
                element(reader)
            });
        }
    }
    public on(proto: Proto, cb: (reader: Reader) => void): ClientHelper {
        let ls = this.eventMap.get(proto)
        if (ls == undefined) {
            ls = []
            this.eventMap.set(proto, ls)
        }
        ls.push(cb)
        return this
    }
}