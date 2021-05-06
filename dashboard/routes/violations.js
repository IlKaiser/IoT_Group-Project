const express = require("express");
const router = express.Router();
const {quickSort} = require("../helper/quickSort");
const {docClient, paramsViolation} = require("../startup/db");

let logged = "";

router.get("/AllViolations", async (req, res) => {
	
  if(logged == "true"){
	  
	  timestamps = [];
  
  docClient.scan(paramsViolation, (err, data) => {
	  if (err) {
        console.error("Unable to scan violation table. Error JSON:", JSON.stringify(err, null, 2));
        return res.status(404).send("Unable to scan the table");
    } else {
        
        for(let i=0;i<data.Items.length;i++){
			timestamps.push(data.Items[i].sample_time);
			data.Items[i].sample_time = new Date(data.Items[i].sample_time);
		}
		
		const sortedTimestamps = quickSort(timestamps, 0, timestamps.length - 1);
		let toReturn = [];

		sortedTimestamps.forEach(t => {
			for(let i=0;i<data.Items.length;i++){
				if(t === (+ data.Items[i].sample_time)){
					
					let month = data.Items[i].sample_time.getMonth();
					let day = data.Items[i].sample_time.getDate();
					const year = data.Items[i].sample_time.getFullYear();
					
					let hour = data.Items[i].sample_time.getHours();
					let minutes = data.Items[i].sample_time.getMinutes();
					let seconds = data.Items[i].sample_time.getSeconds();
					
					if(parseInt(month.toString()) == 12)
					    month = "01";
					    
					else{
					    month = parseInt(month.toString()) + 1;
					
					    if(month.toString().length == 1)
					        month = "0" + month;
    	
					}
										
					if(day.toString().length == 1)
					    day = "0" + day;
					if(hour.toString().length == 1)
					    hour = "0" + hour;
					if(minutes.toString().length == 1)
					    minutes = "0" + minutes;
					if(seconds.toString().length == 1)
					    seconds = "0" + seconds;
					
					const time = month + "/" + day + "/" + year + " " + hour + ":" + minutes + ":" + seconds; 
					
					const data_obj = {
						"station_id" : data.Items[i].device_data.station_id,
						"violation_id" : data.Items[i].device_data.violation_id,
						"time" : time, 
					    "lat" : data.Items[i].device_data.lat,
					    "long" : data.Items[i].device_data.long,
					    "url": data.Items[i].device_data.url   // [TO DO] WIP
					};
					
					toReturn.push(data_obj);
					break;
				}
			}
		});
		
        
        
        return res.status(200).send(toReturn);
    }
  });
	  
  }
  
  else{
	res.status(401).redirect("/");  
  }
  
  
});

router.post("/setLogged", async (req, res) => {
    logged = req.body.logged;
    res.status(200).send("Logged set!");
})

module.exports = router;
