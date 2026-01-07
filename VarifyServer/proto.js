//路径库
const path = require('path')
//grpc库
const grpc = require('@grpc/grpc-js')
//@grpc/grpc-js 只能处理JS对象，所以需要先用proto-loader把.proto文件解析成JS
const protoLoader = require('@grpc/proto-loader')
//__dirname表示当前文件所在的目录，.join拼接字符串，__dirname两个下划线是内置变量
const PROTO_PATH = path.join(__dirname, 'message.proto')
//js对象同步加载.proto 文件
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })
//使用 grpc.loadPackageDefinition 把 packageDefinition 转换成 gRPC 可用的对象
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

//获取proto文件中的所以message和服务信息给一个变量方便使用
const message_proto = protoDescriptor.message
//导出这个包，其他文件可以直接用 require() 拿到服务和消息类型
module.exports = message_proto