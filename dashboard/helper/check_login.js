const { MongoClient } = require("mongodb");
const secret = require('../secret.json');
var crypto = require('crypto');

var hash = crypto.createHash('sha256');
const password = secret.password

function check(nameReq,passwordReq,callback_true,callback_false){
    
    const uri = "mongodb+srv://safetyFloater:"+password+"@cluster0.cc40d.mongodb.net/access?retryWrites=true&w=majority";
    
    
    const client = new MongoClient(uri, { useNewUrlParser: true, useUnifiedTopology: true });


    async function run() {
        try {

            await client.connect();
            console.log("connected");
            const database = client.db('access');
            const inv = database.collection('inventory');
            const query = { 'username': nameReq };
            const user = await inv.findOne(query);
            
            if (user !== null && 
                user.password == crypto.createHash('sha256').update(passwordReq+user.salt).digest("hex")){
                callback_true();
            }else{
                callback_false();
                console.log("login failed");
            }
        
        }catch(e){

            console.log("an error occurred");
            callback_false();
            console.log(e.stack);

        } finally {

            // Ensures that the client will close when you finish/error
            await client.close();
            
        }
    }
    run();
}
module.exports = {
    check
}