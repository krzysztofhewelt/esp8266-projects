const express = require("express")
const client = require("./src/services/db.service")
require("dotenv").config()

const app = express()
const port = process.env.APP_PORT || 8000

// it's rest api so we use only json
app.use(express.json())

// include all needed API routes and CORS settings
app.use(require("./routes"))

// App listening and mongodb connect
client
    .connect(process.env.MONGODB_CONNECTION_STRING)
    .then(() =>
        app.listen(port, () => {
            console.log("Successfully connected to Mongo database!")
            console.log(`App now listening on port ${port}`)
        })
    )
    .catch((err) => {
        console.error(
            "An error occurred during connecting to the Mongo database:"
        )
        console.error(err)
        process.exit(1)
    })

// Close database connection on app stop
process.on("SIGINT", function () {
    client.close()
    process.exit(1)
})
