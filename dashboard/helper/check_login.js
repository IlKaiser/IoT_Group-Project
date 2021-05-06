const { MongoClient } = require("mongodb");
const secret = require('../secret.json');
const config = require("config");
var crypto = require('crypto');

const hash_function = config.get("hash_function");

var hash = crypto.createHash(hash_function);
const password = secret.password

function check(nameReq,passwordReq,callback_true,callback_false){
    
    const uri = config.get("db_safety_floater_base_url")+password+config.get("mongo_db_final_url");
    
    
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
                user.password == crypto.createHash(hash_function).update(passwordReq+user.salt).digest("hex")){
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
