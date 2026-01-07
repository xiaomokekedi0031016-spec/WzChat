//引入 Node.js 的文件系统模块 fs，用于读取文件
const fs = require('fs');
//读取当前目录下的 config.json 文件。使用 utf8 编码读取为字符串。使用JSON.parse解析成 JavaScript对象。
let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
let email_user = config.email.user;
let email_pass = config.email.pass;
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;
let code_prefix = "code_";

module.exports = {email_pass, email_user, mysql_host, mysql_port,redis_host, redis_port, redis_passwd, code_prefix}