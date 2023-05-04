const { MongoClient } = require("mongodb")

let client = null
async function connect(url) {
    if (client == null) {
        client = await MongoClient.connect(url)
    }
}

// get database using pre-established connection
function db(dbName) {
    return client.db(dbName)
}

// close open connection
function close() {
    if (client) {
        client.close()
        client = null
    }
}

// export connect(), db() and close() from the module
module.exports = {
    connect,
    db,
    close,
}
