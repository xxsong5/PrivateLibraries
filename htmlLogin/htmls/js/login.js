$(document).ready(function(){
	//init globla param
	var logchk=[false,false];//for login check
	var regchk=[false,false,false,false,false];//for register check
	$("#gobtn1").click(function(){
		setTimeout(function(){
			$("#logdiv").hide();
			$("#regdiv").show();
		}, 300);
	});
	$("#gobtn2").click(function(){
		setTimeout(function(){
			$("#regdiv").hide();
			$("#logdiv").show();
		}, 300);
	});
	//now login
	$("#logbtn").click(function(){
		login_chk();
		if(logchk[0]==true && logchk[1]==true){
			$("#logbtn").attr("class","btn disabled btn-success");
			login();
		}   			
	});
	//now register
	$("#regbtn").click(function(){
		reg_chk();
		if(regchk[0]==true && regchk[1]==true && regchk[2]==true && regchk[3]==true && regchk[4]==true){
			$("#regbtn").attr("class","btn disabled btn-success");
			register();
		}   			
	});
	//login div lose focus
	$("#log_un").blur(function(){
		login_un_hide();
		var log_un=$("#log_un").val();
		if(log_un==null || log_un==""){
			$("#log_un_1").show();
		}else{
			$("#log_un_2").show();
			$("#log_un").val($("#log_un").val().replace(/['<>:"]/g, ""));
		}
	});
	$("#log_pwd").blur(function(){
		login_pwd_hide();
		var log_pwd=$("#log_pwd").val();
		if(log_pwd==null || log_pwd=="" || log_pwd.length<6){
			$("#log_pwd_1").show();
		}else{
			$("#log_pwd_2").show();
		}
	});
	//register div lose focus
	$("#reg_un").blur(function(){
		reg_un_hide();
		var reg_un=$("#reg_un").val();
		if(reg_un==null || reg_un==""){
			$("#reg_un_1").show();
		}else{
			var unck=/^([a-z]|[A-Z]|[0-9]|[._@])+$/;
			if(!unck.test(reg_un)){
				$("#reg_un_1").show();
			}else{
				$("#reg_un_2").show();
			}
		}
	});
	$("#reg_pwd").blur(function(){
		reg_pwd_hide();
		var reg_pwd=$("#reg_pwd").val();
		if(reg_pwd==null || reg_pwd=="" || reg_pwd.length<6){
			$("#reg_pwd_1").show();
		}else{
			$("#reg_pwd_2").show();
		}
	});
	$("#reg_pwd1").blur(function(){
		reg_pwd1_hide();
		var reg_pwd=$("#reg_pwd").val();
		var reg_pwd1=$("#reg_pwd1").val();
		if(reg_pwd1==null || reg_pwd1=="" || reg_pwd1.length<6){
			$("#reg_pwd_3").show();
		}else{
			if(reg_pwd1!=reg_pwd){
				$("#reg_pwd_5").show();
			}else{
				$("#reg_pwd_4").show();
			}
		}
	});
	$("#reg_mail").blur(function(){
		reg_mail_hide();
		var reg_mail=$("#reg_mail").val();
		if(reg_mail==null || reg_mail==""){
			$("#reg_mail_1").show();
		}else{
			var mailck = /^([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+@([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+\.[a-zA-Z]{2,3}$/;
			if(!mailck.test(reg_mail)){
			    $("#reg_mail_1").show();
			}else{
				$("#reg_mail_2").show();
			}
		}
	});
	$("#reg_chk").blur(function(){
		reg_chk_hide();
		if($("#reg_chk").attr("checked")!="checked"){
			$("#reg_chk_1").show();
		}
	});
	//login check
	function login_chk(){
		login_un_hide();
		login_pwd_hide();
		var log_un=$("#log_un").val();
		var log_pwd=$("#log_pwd").val();
		if(log_un==null || log_un==""){
			$("#log_un_1").show();
			logchk[0]=false;
		}else{
			$("#log_un_2").show();
			logchk[0]=true;
		}
		if(log_pwd==null || log_pwd=="" || log_pwd.length<6){
			$("#log_pwd_1").show();
			logchk[1]=false;
		}else{
			$("#log_pwd_2").show();
			logchk[1]=true;
		}
	}
	//register check
	function reg_chk(){
		reg_un_hide();
		reg_pwd_hide();
		reg_pwd1_hide();
		reg_mail_hide();
		reg_chk_hide();
		var reg_un=$("#reg_un").val();
		var reg_pwd=$("#reg_pwd").val();
		var reg_pwd1=$("#reg_pwd1").val();
		var reg_mail=$("#reg_mail").val();
		//reg username
		if(reg_un==null || reg_un==""){
			$("#reg_un_1").show();
			regchk[0]=false;
		}else{
			var unck=/^([a-z]|[A-Z]|[0-9]|[._@])+$/;
			if(!unck.test(reg_un)){
				$("#reg_un_1").show();
				regchk[0]=false;
			}else{
				$("#reg_un_2").show();
				regchk[0]=true;
			}
		}
		//reg password
		if(reg_pwd==null || reg_pwd=="" || reg_pwd.length<6){
			$("#reg_pwd_1").show();
			regchk[1]=false;
		}else{
			$("#reg_pwd_2").show();
			regchk[1]=true;
		}
		//reg password sure
		if(reg_pwd1==null || reg_pwd1=="" || reg_pwd1.length<6){
			$("#reg_pwd_3").show();
			regchk[2]=false;
		}else{
			if(reg_pwd1!=reg_pwd){
				$("#reg_pwd_5").show();
				regchk[2]=false;
			}else{
				$("#reg_pwd_4").show();
				regchk[2]=true;
			}
		}
		//reg email
		if(reg_mail==null || reg_mail==""){
			$("#reg_mail_1").show();
			regchk[3]=false;
		}else{
			var mailck = /^([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+@([a-zA-Z0-9]+[_|\_|\.]?)*[a-zA-Z0-9]+\.[a-zA-Z]{2,3}$/;
			if(!mailck.test(reg_mail)){
			    $("#reg_mail_1").show();
				regchk[3]=false;
			}else{
				$("#reg_mail_2").show();
				regchk[3]=true;
			}
		}
		//fitband service
		if($("#reg_chk").attr("checked")!="checked"){
			$("#reg_chk_1").show();
			regchk[4]=false;
		}else{
			regchk[4]=true;
		}
	}
	function login(){
		var rand=Math.random();
		$.ajax({
			url:"user_login.do?params="+rand,
			dataType:"json",
			data:{username:$("#log_un").val(),userpwd:$("#log_pwd").val()},
			success:function(data){
				if(data.message=='0'){
					//that you are fool
				}
				if(data.message=='1'){
					$("#logbtn").attr("class","btn btn-success");
					login_un_hide();
					$("#log_un_3").show();
				}
				if(data.message=='2'){
					$("#logbtn").attr("class","btn btn-success");
					login_pwd_hide();
					$("#log_pwd_3").show();
				}
				if(data.message=='3'){
					SetLogCookie($("#log_un").val(),$("#log_pwd").val());
					setTimeout(function(){
						window.location.href="/index.jsp";
					}, 500);
				}
			}
		});
	}
	function register(){
		var rand=Math.random();
		$.ajax({
			url:"user_register.do?params="+rand,
			dataType:"json",
			data:{
				username:$("#reg_un").val(),
				userpwd:$("#reg_pwd").val(),
				useremail:$("#reg_mail").val()
			},
			success:function(data){
				if(data.message!=null){
					if(data.message=="0"){
						//that you are fool
					}
					if(data.message=="1"){
						//that you are fool
					}
					if(data.message=="2"){
						//that you are fool
					}
					if(data.message=="3"){
						reg_un_hide();
						$("#reg_un_3").show();
						$("#regbtn").attr("class","btn btn-success");
					}
					if(data.message=="4"){
						reg_mail_hide();
						$("#reg_mail_3").show();
						$("#regbtn").attr("class","btn btn-success");
					}
				}else{
					SetLogCookie($("#reg_un").val(),$("#reg_pwd").val());
					setTimeout(function(){
						window.location.href="/index.jsp";
					}, 500);
				}
			}
		});
	}
	//hide login tips
	function login_un_hide(){
		$("#log_un_1").hide();$("#log_un_2").hide();$("#log_un_3").hide();
	}
	function login_pwd_hide(){
		$("#log_pwd_1").hide();$("#log_pwd_2").hide();$("#log_pwd_3").hide();
	}
	//hide register tips
	function reg_un_hide(){
		$("#reg_un_1").hide();$("#reg_un_2").hide();$("#reg_un_3").hide();
	}
	function reg_pwd_hide(){
		$("#reg_pwd_1").hide();$("#reg_pwd_2").hide();
	}
	function reg_pwd1_hide(){
		$("#reg_pwd_3").hide();$("#reg_pwd_4").hide();$("#reg_pwd_5").hide();
	}
	function reg_mail_hide(){
		$("#reg_mail_1").hide();$("#reg_mail_2").hide();$("#reg_mail_3").hide();
	}
	function reg_chk_hide(){
		$("#reg_chk_1").hide();
	}
	LoadCookie();
});
