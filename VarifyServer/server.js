const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
//const { v4: uuidv4 } ： 从uuid模块中取出v4把它重命名为uuidv4
const { v4: uuidv4 } = require('uuid');//用于生成128字节的字符串
const emailModule = require('./email');
const redis_module = require('./redis')

//rpc函数调用
async function GetVarifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        let query_res = await redis_module.GetRedis(const_module.code_prefix+call.request.email);
        console.log("query_res is ", query_res)
        let uniqueId = query_res;
        if(query_res ==null){
            uniqueId = uuidv4();
            if (uniqueId.length > 4) {
                uniqueId = uniqueId.substring(0, 4);
            } 
            //往redis里面插入数据
            let bres = await redis_module.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId,60)
            if(!bres){
                callback(null, { email:  call.request.email,
                    error:const_module.Errors.RedisErr
                });
                return;
            }
        }

        console.log("uniqueId is ", uniqueId)
        let text_str =  '恭喜您抽到了圣诞限定飞机杯，您的验证码为'+ uniqueId +'请一分钟内完成注册，登录www.91prony.com领取您的福利，我们会在一天内发货感谢您的参与'
        //发送邮件
        let mailOptions = {
            from: 'u91porny@163.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
    
        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send res is ", send_res)

        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success//这里直接就是返回给rpc客户端的数据
            ,code:uniqueId 
        }); 
        
 
    }catch(error){
        console.log("catch error is ", error)

        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
     
}
function main() {
    //grpc服务端
    var server = new grpc.Server()
    //{ GetVarifyCode: GetVarifyCode}方法映射
    //当客户端调用 VarifyService 服务里的 GetVarifyCode 方法时，实际要执行我自己写的函数 GetVarifyCode()。
    server.addService(message_proto.VarifyService.service, { GetVarifyCode: GetVarifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')        
    })
}

main()