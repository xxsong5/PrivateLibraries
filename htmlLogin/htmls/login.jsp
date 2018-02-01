
<!DOCTYPE html>
<html lang="zh-cn">
  <head>
  	<meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,Chrome=1">
    <title>FITBAND官网-登录</title>
    <script type="text/javascript" src="/weme/js/jquery-1.8.3.min.js"></script>
    <script type="text/javascript" src="/weme/js/bootstrap.min.js"></script>
	<!--[if lt IE 9]>
	  <script type="text/javascript" src="/weme/js/jquery.placeholder.js"></script>
	<![endif]-->
	<link rel="stylesheet" href="/weme/css/bootstrap.min.css" type="text/css">
	<link rel="stylesheet" href="/weme/css/font-awesome.min.css" type="text/css">
	<link rel="stylesheet" href="/weme/css/ace.min.css" type="text/css">
    <link rel="stylesheet" href="/weme/css/index.css" type="text/css">
	<script type="text/javascript" src="/weme/js/login.js"></script>
    <script type="text/javascript" src="/weme/js/cookie.js"></script>
<!--     <script type="text/javascript" src="/weme/js/global.js"></script> -->
	<script type="text/javascript">
		$(document).ready(function(){
			
		});
	</script>
<style type="text/css">
body{overflow:hidden;}
a img{border:none;}
a{display:inline-block;}
.gridContainer{width:1920px; margin-left:-960px;position:absolute;left:50%;height: 100%;overflow: hidden;}
</style>
  </head>
  
  <body>
   	<!-- 主体 -->
   	<div class="gridContainer">
   		<img style="" src="image/login_bg.jpg">
   		<div style="width: 100%;float: left;color: #5e5e5e;position: absolute;top: 0;">
   			<div style="width: 960px;margin: 0 auto;">
   				<div style="width: 960px;float: left;margin-top: 70px;">
   					<a href="/index.jsp"><img style="width: 234px;height: 54px;float: left;margin-left: 363px;" src="image/login_fitband.png"></a>
   				</div>
   				<div style="width: 960px;float: left;height: 50px;font-size: 22px;text-align: center;line-height: 58px;">
   					一个账号掌管你的健康数据
   				</div>
   				<div style="width: 960px;float: left;height: 30px;font-size: 14px;text-align: center;">
   					我的排名、运动记录、睡眠报告、礼品兑换，一应俱全
   				</div>
   				<!-- login div -->
   				<div id="logdiv" style="width: 960px;float: left;margin-top: 70px;">
   					<div style="width: 960px;float: left;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="text"  id="log_un" maxlength="20" style="width: 300px;" placeholder="账号">
	   						<i class="icon-user"></i>
	   					</span>
	   					<span id="log_un_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>账号必须由英文字母或数字组成</span>
	   					<span id="log_un_2" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
	   					<span id="log_un_3" style="color: #d16e6c;display: none;"><i class="icon-remove"></i>账号不存在</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="password" id="log_pwd" maxlength="20" style="width: 300px;" placeholder="密码">
	   						<i class="icon-lock"></i>
	   					</span>
	   					<span id="log_pwd_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>密码的长度应该在6位以上</span>
	   					<span id="log_pwd_2" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
	   					<span id="log_pwd_3" style="color: #d16e6c;display: none;"><i class="icon-remove"></i>密码错误</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 30px;">
   						<a href="index_losepwd.jsp" style="float: left;margin-left: 560px;color: #5e5e5e;">忘记密码？</a>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 30px;">
   						<button id="logbtn" class="btn btn-success" style="width: 300px;margin-left: 330px;"><i class="icon-key"></i>登录</button>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
	   					<button id="gobtn1" class="btn" style="width: 300px;margin-left: 330px;"><i class="icon-edit"></i>注册</button>
   					</div>
   				</div>
   				<!-- register div -->
   				<div id="regdiv" style="width: 960px;float: left;margin-top: 70px;display: none;">
   					<div style="width: 960px;float: left;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="text"  id="reg_un" maxlength="20" style="width: 300px;" placeholder="账号">
	   						<i class="icon-user"></i>
	   					</span>
	   					<span id="reg_un_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>账号必须由英文字母或数字组成</span>
	   					<span id="reg_un_2" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
	   					<span id="reg_un_3" style="color: #d16e6c;display: none;"><i class="icon-remove"></i>账号已被注册</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="password" id="reg_pwd" maxlength="20" style="width: 300px;" placeholder="密码">
	   						<i class="icon-lock"></i>
	   					</span>
	   					<span id="reg_pwd_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>密码的长度应该在6位以上</span>
	   					<span id="reg_pwd_2" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="password" id="reg_pwd1" maxlength="20" style="width: 300px;" placeholder="确认密码">
	   						<i class="icon-lock"></i>
	   					</span>
	   					<span id="reg_pwd_3" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>确认密码的长度应该在6位以上</span>
	   					<span id="reg_pwd_4" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
	   					<span id="reg_pwd_5" style="color: #d16e6c;display: none;"><i class="icon-remove"></i>两次输入不一致</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
   						<span class="input-icon" style="margin-left: 330px;">
	   						<input type="text" id="reg_mail" maxlength="32" style="width: 300px;" placeholder="邮箱">
	   						<i class="icon-envelope"></i>
	   					</span>
	   					<span id="reg_mail_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>邮箱格式错误</span>
	   					<span id="reg_mail_2" style="color: #7ba065;display: none;"><i class="icon-ok"></i>填写正确</span>
	   					<span id="reg_mail_3" style="color: #d16e6c;display: none;"><i class="icon-remove"></i>邮箱已被注册</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 20px;">
   						<span style="margin-left: 330px;width: 304px;float: left;"><input type="checkbox" id="reg_chk" class="ace" checked="checked"><span class="lbl"></span>我已阅读并同意<a href="weme_service.jsp">Fitband服务条款</a></span>
   						<span id="reg_chk_1" style="color: #d16e6c;display: none;"><i class="icon-exclamation-sign"></i>必须同意Fitband服务条款</span>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 30px;">
   						<button id="regbtn" class="btn btn-success" style="width: 300px;margin-left: 330px;"><i class="icon-edit"></i>注册</button>
   					</div>
   					<div style="width: 960px;float: left;margin-top: 10px;">
	   					<button id="gobtn2" class="btn" style="width: 300px;margin-left: 330px;"><i class="icon-key"></i>已有账号，直接登录</button>
   					</div>
   				</div>
   			</div>
   		</div>
   	</div>
  </body>
</html>
