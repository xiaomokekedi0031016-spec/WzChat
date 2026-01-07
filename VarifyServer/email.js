//nodejs发送邮件库
const nodemailer = require('nodemailer');
const config_module = require("./config")

/**
 * 创建发送邮件的代理
 */

//创建一个发送邮件的“传输器”（transport）。这个对象封装了与 SMTP 服务器的连接和发送逻辑，后续调用 transport.sendMail() 来发邮件。
let transport = nodemailer.createTransport({
    host: 'smtp.163.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user, // 发送方邮箱地址
        pass: config_module.email_pass // 邮箱授权码或者密码
    }
});

/**
 * 发送邮件的函数
 * @param {*} mailOptions_ 发送邮件的参数
 * @returns 
 */
function SendMail(mailOptions_){
    //返回异步结果，通过resolve()或reject()告诉外部结果，外部代码可以用 .then() 和 .catch() 来接收结果
    return new Promise(function(resolve, reject)
    {
        transport.sendMail(mailOptions_, function(error, info)
        {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                //info.response 通常包含 SMTP 服务器返回的短信息
                console.log('邮件已成功发送：' + info.response);
                resolve(info.response)
            }
        });
    })
   
}

module.exports.SendMail = SendMail