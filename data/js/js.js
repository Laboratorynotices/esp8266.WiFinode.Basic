$.urlParam = function(name){
    var results = new RegExp('[\?&]' + name + '=([^&#]*)').exec(window.location.href);
    if (results==null){
       return null;
    }
    else{
       return decodeURI(results[1]) || 0;
    }
}



$( document ).ready(function() {
	switch($.urlParam('site')) {
		case "config":
			$('#content').load('./config.htm');
			break;
		case "main":
		default:
			$('#content').load('./start.htm');
	}
});