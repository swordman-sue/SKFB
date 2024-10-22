<?php
header("Content-type: text/html; charset=utf-8");
/**
 *功能：配置文件
 *版本：1.0
 *修改日期：2014-06-26
 '说明：
 '以下代码只是为了方便商户测试而提供的样例代码，商户可以根据自己的需要，按照技术文档编写,并非一定要使用该代码。
 '该代码仅供学习和研究爱贝云计费接口使用，只是提供一个参考。
 */
 

//爱贝商户后台接入url
// $coolyunCpUrl="http://pay.coolyun.com:6988";
$iapppayCpUrl="http://ipay.iapppay.com:9999";
//登录令牌认证接口 url
$tokenCheckUrl=$iapppayCpUrl . "/openid/openidcheck";

//下单接口 url
// $orderUrl=$coolyunCpUrl . "/payapi/order";
$orderUrl=$iapppayCpUrl . "/payapi/order";

//支付结果查询接口 url
$queryResultUrl=$iapppayCpUrl ."/payapi/queryresult";

//契约查询接口url
$querysubsUrl=$iapppayCpUrl."/payapi/subsquery";

//契约鉴权接口Url
$ContractAuthenticationUrl=$iapppayCpUrl."/payapi/subsauth";

//取消契约接口Url
$subcancel=$iapppayCpUrl."/payapi/subcancel";
//H5和PC跳转版支付接口Url
$h5url="https://web.iapppay.com/h5/gateway?";
$pcurl="https://web.iapppay.com/pc/gateway?";

/*
//应用编号
$appid="3002495803";
//应用私钥
$appkey="MIICXgIBAAKBgQDZjpotEmPKwRPKeqb6kqLgmiJkmUQ6EgMAlHBLCAdykb8mAKNWPFs+uyyknOAg+kqQzS9oEoF1P2YYDzYLVQeU4x2c9PtLPMxhXOqdiS6tdJ7RBa2SS4z0WaPmjGAds8qvVnec8Kp5/UXpIDXHRfJ7vNCwNB5O1BEJO/uIRMNIOwIDAQABAoGBAM/HCOpo+NPIyN0FfPotF8/IhXZshqOrViC0o/aU6X/7QILL8zNGG6Ly4nUouknkoVhgDpmnqupOrXPm+yehgsVljoeYRDVEmoyPvFNm+lbv5iDsnHlOohkyEdIO8tMYX7FT269YjWd7PGV1cnHqYUUAq0ncxbu2/RbPbXXsWcAJAkEA83tyOnvKMm46wEaLYXbUzS2iWCv+X0/4h5IKU9q9D2kYhApGaPIOWKf7qtLigkwvSwZ6ZU4kPKcmM817WR64vwJBAOS98iTBD6tYMe6U0Y5E1rO/sQQrIpMwJ2Y6PJQ/uU8z6rXsbEztUR/pbEExkf2a0xCByRxPEwrIRgzBat2Q84UCQHvAvLhY/tZPDHF56ZHqMhLvJNqn0axkGy/c3H7uaLWSdzF1f4ALt5r8FoAmm5YaXtdFPaSL6QMi+dnOkOklIkUCQQCFt0AhGjb1vCXcSWTDHRzBkRKC1FBu6JxvlyWoqCPE2B2h4aZhxe1BkWu2JKsqLGKr6KLPCK6iA/dnJ344La8dAkEA71Mz9WB4fhhU3e284lM1Y6ybS+F5a04racip9NbRAp9bnaDXDBO/EeFM4JZtb48IgnIJ0hfe09y+MDFgw4tgFA==";
//平台公钥
$platpkey="MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDoEQasj75Yi/MWc0ZF0+ZiBmguwSegyr5Z+N5A93VXG7SSA4OSb/eaH5FgCsW25cGxRwfpwl9jamiG4R/2vM72cQFQX5lEK0JkcHd/gxhhGFwGg7snFJpYCxYCflN0WHxrR47rijC7ipzRp+Noq50AMCF79AMaPIRSTCy9gLPpwQIDAQAB";
*/


//应用编号
$appid="3018063546";
//应用私钥
$appkey="MIICXAIBAAKBgQDXmatuBPFLhsB7P6srHob65f8NgzUCOiU8AcVaGDm2Jbi6r7FVYDuJ9zcGW0YplAY+QpQMBuz3MC/gBLMFlanEMVBxX/Bz012m4p+19fpE2kH8TMkh1WhBPdpGh4QD51hBpj8nWT54BvinnK4HZnCDY7PifLAimprv62MEbvy1jwIDAQABAoGBAMXpSnS0FMw/5nE9AOOUIlQuzPKafw6MUaHD/mW110RdXDYDEcZNkFoVlW1AXCfOuextMXnY4a4HjesWZT+7IxpG1CuVb06mVIp0jXelCX6fMWJuZDk4nudIzU9xO7NiXocVWk7nSUnEZU5OVlz3rQ2NrL01Qaclf7nbRIIFrmgBAkEA7N9xuhhd0TyVAhDLbcOuxLhJqzI3T/lgvp62eWAAdjeh75iXkfRDRAtUuncT/0de47HkQbxxrPe1Y8sBG3J/rQJBAOkCfHmcSjSifwwH62gSfE+dTKQDI5cIV1c/RbZmTwZLNHaCL6KxfJngK/3WW6ZNEbshRBO9X+BmXQCFGMxAwasCQAUOdAgL8xy/VVL49s6+CaILaQ8tgD7FzQnkq3KF2Rys7VkHAd1MwI7YSxO77QJbDKyBpKS+mBKgKxAZSknCtPUCQBZ/nvU/GgsrBYlr2lcaMdaeIaBUjUMwEv7hpSGPj2HUMSSgPWlAKf/RnW3NHNxZ5LRk6eXeCzvnpmM+dr6CIjECQDU2T3SZKzmuDuHPaEE73t5JyRKNvEhCqGIPoEv3knmqHv+ZzxCmIsGbnQutXnNtT+wPEVY502H2yh6i5UDstO4=";
//平台公钥
$platpkey="MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCzTISN4/5s7KhsqUefV5OGtTtBGnKjFHyeQ1N16NdUZhP+qOFhZWTNKdsW6jxUskZDL2KVSVFPTJ4W0zy1wjGFymqACh3dL7u5OKxU4FOuLY0CYTVHpKb+XFjhpW+C0NuD5GWhUigDeON5ANTTLCcoTPPeaAaZi+JZY4mEv1wBzwIDAQAB";


?>