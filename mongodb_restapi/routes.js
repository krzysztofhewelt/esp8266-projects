const measurements = require("./src/routes/measurements.route")
const leds = require("./src/routes/leds.route")
const cors = require("cors")
const MainRouter = require("express").Router()

// CORS: Add Grafana to origin
MainRouter.use(cors({ credentials: true, origin: "http://localhost:3000" }))

// Main page -> tell that we are alive
MainRouter.get("/", (req, res) => {
    res.json({ message: "Nothing to show" })
})

// API routes
MainRouter.use("/api/measurements", measurements)
MainRouter.use("/api/leds", leds)

// Not found error
MainRouter.use((req, res) => {
    res.status(404).json({ error: "Not found" })
})

// Error handler
MainRouter.use((err, req, res) => {
    const statusCode = err.statusCode || 500
    console.error(err.message, err.stack)
    res.status(statusCode).json({ message: err.message })
})

module.exports = MainRouter
