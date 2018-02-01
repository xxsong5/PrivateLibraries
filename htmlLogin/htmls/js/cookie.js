function DelLogCookie(){DeleteCookie('ds_fb_name');DeleteCookie('ds_fb_pwd');}
function SetLogCookie(username,password){SetCookie('ds_fb_name',username);SetCookie('ds_fb_pwd',password);}	
function LoadCookie(){
    var username=GetCookie('ds_fb_name'),password=GetCookie('ds_fb_pwd');
    if(username!=null && username!="" && username!="null"){$("#log_un").val(username);}
    if(password!=null && password!="" && password!="null"){$("#log_pwd").val(password);}
}
function GetCookie(name){var arg = name + "=";var alen = arg.length;var clen = document.cookie.length;var i = 0;while (i < clen){var j = i + alen;if (document.cookie.substring(i, j) == arg) return getCookieVal(j);i = document.cookie.indexOf(" ", i) + 1;if (i == 0)break;}return null;}
function getCookieVal(offset){var endstr = document.cookie.indexOf (";", offset);if (endstr == -1)endstr = document.cookie.length;return unescape(window.document.cookie.substring(offset, endstr));}
function SetCookie(name,value){var exp = new Date();exp.setTime(exp.getTime()+(30*24*60*60*1000));document.cookie = name + "=" + escape (value) + "; expires=" + exp.toGMTString()+";path=/";}
function DeleteCookie(name){var exp = new Date();exp.setTime (exp.getTime() - 100);var cval = null;document.cookie = name + "=" + cval +";path=/";}
