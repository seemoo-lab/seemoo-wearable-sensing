var SAAgent,
    SASocket,
    connectionListener,
    responseTxt = document.getElementById("responseTxt");
var sensors;
var stream, camcontrol;
var running = false;

/* Make Provider application running in background */
tizen.application.getCurrentApplication().hide();

function createHTML(log_string)
{
    var content = document.getElementById("toast-content");
    content.innerHTML = log_string;
    tau.openPopup("#toast");
}


/*
 * Most likly used for the Service app to be debugged
 */
function messageCB(data, replyPort){
	//printAllFunctions(data);
	var out = "";
	for(var i = 0; i< data.length; i++){
		
		//printAllFunctions(obj);
		//printAllFunctions(data[i]);
		if(typeof data[i].value == "object"){
			for (var j = 0; j < data[i].value.length; j++){
				out+= data[i].value[j];
				if (j != data[i].value.length-1) out+= " ";
			}
		} else {
			out+= data[i].value.toString();
		}
		
		
		if(i != data.length-1){
			out+= " | ";
		}
	}
	//TODO: send data to phone but check if it is availble
	//SASocket.sendData(SAAgent.channelIds[0], out);
	if (outerHeight === "true"){
		console.log("Service is running");
		var btn = document.getElementById("startBtn");
		btn.innerHTML = "Stop Recording";
		document.getElementById("time").innerHTML = "is still recording";
	}
	
	if (out === "service_app_terminate"){
		//console.log("Terminated");
	}
	console.log(out);
	document.getElementById("time").innerHTML = out;
	
}

var appControlReplyCallback = {
		// callee sent a reply
		onsuccess: function(data) {
			console.log(JSON.stringify(data));

		},
		// callee returned failure
		onfailure: function() {
			console.log('The launch application control failed');
		}
}

function stopTimer(){
	clearInterval(timer);
}

function addSecond(){
	date.setSeconds(date.getSeconds()+1,date.getMilliseconds());
	//date.setMinutes(date.getMinutes()+1, date.getSeconds(), date.getMilliseconds());
	var hour = date.getUTCHours();
	document.getElementById("time").innerHTML = date.toLocaleTimeString("de-DE");
	if (hour == 8) {
		stopSensors();
	}
}
var timer;
var date
var alarm;

/*
 * You might dont want to ust this since it might let the app crash.....
 * without everything works fine
 */
function startTimer(){
	date = new Date(0);
	date.setHours(0, 0, 0, 0);
	//timer = setInterval(addSecond, 1000);
	//document.getElementById("time").innerHTML = date.toLocaleTimeString("de-DE");
	
	//var stopdate = new Date(0);
	/*stopdate.setHours(stopdate.getHours()+1, stopdate.getMinutes(), stopdate.getSeconds(), stopdate.getMilliseconds());
	alarm = new tizen.AlarmAbsulte(stopdate);
	tizen.alarm.add(alarm, applicationId, appControl)*/
}


var running = false;


/*
 * The only important part to be executed Since everything is done in the Service App.
 * IMPORTANT! the timer in JavaScript might bring the app to crash after a few hours which is bad if you wnat to let it run for upto 8 hours
 */
function showSensors(){
	console.log(running);
	if (running){
		running = !running;
		console.log("stopping");
		var btn = document.getElementById("startBtn");
		btn.innerHTML = "Start Recording";
		stopSensors();
		return;
	}
	running = !running;
	console.log("Starting");
	
	var btn = document.getElementById("startBtn");
	btn.innerHTML = "Stop Recording";

	
	var localPort = tizen.messageport.requestLocalMessagePort('MY_PORT');
	var localPortWatchId = localPort.addMessagePortListener(messageCB);
	
	//starting native app
	var returnData = new tizen.ApplicationControlData("UM2FcDkLYZ.HelloAccessoryProvider", ["Success"]);

	
	var appControl = new tizen.ApplicationControl("start",
			null,
			null,
			null,
			[returnData] 
	);
	
	tizen.application.launchAppControl(appControl,
			"UM2FcDkLYZ.servicesensor",
			function() {console.log("Launch Service succeeded"); },
			function(e) {console.log("Launch Service failed : " + e.message);},
			appControlReplyCallback);
	
	startTimer();
}


function stopSensors(){
//TODO:  stop everything
var returnData = new tizen.ApplicationControlData("UM2FcDkLYZ.HelloAccessoryProvider", ["Success"]);

	var messagePort = tizen.messageport.requestRemoteMessagePort("UM2FcDkLYZ.servicesensor", "MY_PORT");
	messagePort.sendMessage([{key: "key", value: "stop"}]);
	
	stopTimer();
}

function onSucc(){
	console.log("sensor Running")
}



function getRecordingFormat() {
    return 'pcm';
}

function createAudioFileName() {
    var currentDate = new Date(),
        extension = getRecordingFormat(),
        fileName = '',
        currenttime = tizen.time.getCurrentDateTime();

    fileName = currenttime.toLocaleString() +
        '.' + extension;

    fileName = fileName.replace(/,/g,"");
    fileName = fileName.replace(/:/g,"-");
    fileName = fileName.trim();
    return fileName;
}


function makeRecorderSettings(){
	var settings = {},
    fileName = createAudioFileName(),
	recordingFormat = getRecordingFormat();
	
	
	settings.fileName = fileName;
	settings.recordingFormat = recordingFormat;
	
	return settings;
}

/*
 * Help function to print all functions in an object
 */
function printAllFunctions(obj){
	console.log("For Object: " + obj.constructor.toString());	
	for (var i in obj){	
		if (typeof obj[i] == "string" | typeof obj[i] == "number" | typeof obj[i] == "boolean") {
			console.log(i+" : "+typeof obj[i]+ " : " + obj[i]);
		} else {
			console.log(i+" : "+typeof obj[i]);
		}
	}
}


/*
 * First attempt to record audio which is now all done in C via an Serice app
 */
function startRecorder(){
	//navigator.webkitGetUserMedia({video: false, audio: true}, gotStreamCallback,
      //    noStreamCallback);
	
	//printAllFunctions(navigator);
	//printAllFunctions(navigator.mediaDevices.getSupportedConstraints());
	//printAllFunctions(navigator.mediaDevices);
	//stream = navigator.mediaDevices.getUserMedia({video: false, audio: true});

	function gotStreamCallback(rStream){
		stream = rStream;
		//console.log("Got a stream yaaayyy!!!");
		//console.log(JSON.stringify(stream));
		printAllFunctions(stream);
		var mediaStreamTrack = stream.getAudioTracks()[0];
		printAllFunctions(mediaStreamTrack);
		console.log(mediaStreamTrack.getConstraints());
		
		//printAllFunctions(navigator.tizCamera);
		//printAllFunctions(navigator.mediaDevices);
		//navigator.tizCamera.createCameraControl(stream, gotCameraCallback, noCameraCallback);
		
		
		function gotCameraCallback(cam ) {
			camcontrol = cam;
			printAllFunctions(camcontrol.capabilities);
			console.log("yaaayyy we got the cam");
			//console.log("the formats for recording are: "+camcontrol.capabilities.recordingFormats);
			//console.log(JSON.stringify("supported formats : "+.recordingFormats()));
			//printAllFunctions(camcontrol.recorder);
			camcontrol.recorder.applySettings(makeRecorderSettings(),
                    recorderSettingSuccessCallback,
                    recorderSettingErrorCallback);
			function recorderSettingSuccessCallback(){
				//console.log(JSON.stringify(camcontrol));
				//camcontrol.recorder.start(onStart,onFail);
			
				function onStart(){
					console.log("Nimmt auf ...");
				}
				function onFail(error){
					console.log("fehler beim starten der aufnahme");
					console.log(JSON.stringify(error));
				}
			}
			function recorderSettingErrorCallback(error){
				console.log("settings failed to set");
				console.log(JSON.stringify(error));
			}
		}
		function noCameraCallback() {
			console.log("camera callback failed !!!");
		}
	}
	
	function noStreamCallback() {
		console.log("shiiittt no stream");
	}
}

function stopRecorder (){
	camcontrol.recorder.stop(onSuccess,onFailure);
	
	function onSuccess(){
		stream.stop();
		console.log("stop die aufnahme ....");
	}
	function onFailure(){
		console.log("nope... aufnahme stoppen ging nicht ... shiteeee :D");
	}
}

function sensorListener(sensorName){
	var name = sensorName;

	return onChangeListener;
	
	function onChangeListener(sensorData){
		var data = name + " : " +JSON.stringify(sensorData);
		SASocket.sendData(SAAgent.channelIds[0], data);
		console.log(name + "Data has been send ");
		//console.log(name + " : " +JSON.stringify(sensorData));
		/*for (var ele in sensorData){
			console.log(ele + ": " + sensorData[ele]);
		}*/
	}
}


connectionListener = {
    /* Remote peer agent (Consumer) requests a service (Provider) connection */
    onrequest: function (peerAgent) { 				
   
        createHTML("peerAgent: peerAgent.appName<br />" +
                    "is requsting Service conncetion...");

        /* Check connecting peer by appName*/
        if (peerAgent.appName === "HelloAccessoryConsumer") {
            SAAgent.acceptServiceConnectionRequest(peerAgent);
            createHTML("Service connection request accepted.");

        } else {
        	SAAgent.rejectServiceConnectionRequest(peerAgent);
            createHTML("Service connection request rejected.");

        }
    },

    /* Connection between Provider and Consumer is established */
    onconnect: function (socket) {
        var onConnectionLost,
            dataOnReceive;

        createHTML("Service connection established");

        /* Obtaining socket */
        SASocket = socket;

        onConnectionLost = function onConnectionLost (reason) {
        	//stopSensors();
            createHTML("Service Connection disconnected due to following reason:<br />" + reason);
        };

        /* Inform when connection would get lost */
        SASocket.setSocketStatusListener(onConnectionLost);
        dataOnReceive =  function dataOnReceive (channelId, data) {
        	if(data === "start"){
        		console.log("start has beeen send");
        		//showSensors();
        	}
        }
        SASocket.setDataReceiveListener(dataOnReceive);
        

            },
    onerror: function (errorCode) {
    	//stopSensors()
        createHTML("Service connection error<br />errorCode: " + errorCode);
    }
};

/*
 * Setup of the SSA agent
 */
function requestOnSuccess (agents) {
    var i = 0;
    for (i; i < agents.length; i += 1) {
        if (agents[i].role === "PROVIDER") {
            createHTML("Service Provider found!<br />" +
                        "Name: " +  agents[i].name);
            SAAgent = agents[i];
            break;
        }
    }

    /* Set listener for upcoming connection from Consumer */
    SAAgent.setServiceConnectionListener(connectionListener);
};

function requestOnError (e) {
    createHTML("requestSAAgent Error" +
                "Error name : " + e.name + "<br />" +
                "Error message : " + e.message);
};

/* Requests the SAAgent specified in the Accessory Service Profile */
//Was initially planned to transfer all files to an connected android device but transfering data wasnt fast enough so data is stored now locally 
//webapis.sa.requestSAAgent(requestOnSuccess, requestOnError);


(function () {
    /* Basic Gear gesture & buttons handler */
    window.addEventListener('tizenhwkey', function(ev) {
        var page,
            pageid;

        if (ev.keyName === "back") {
            page = document.getElementsByClassName('ui-page-active')[0];
            pageid = page ? page.id : "";
            if (pageid === "main") {
                try {
                    tizen.application.getCurrentApplication().hide();
                } catch (ignore) {
                }
            } else {
                window.history.back();
            }
        }
    });
}());

(function(tau) {
    var toastPopup = document.getElementById('toast');

    toastPopup.addEventListener('popupshow', function(ev){
        setTimeout(function () {
            tau.closePopup();
        }, 3000);
    }, false);
})(window.tau);