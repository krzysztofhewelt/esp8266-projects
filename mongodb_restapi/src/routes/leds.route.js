const express = require("express")
const router = express.Router()
const leds = require("../controllers/leds.controller")

router.get("/:chipId", leds.get)

router.post("/:chipId/:ledName/:status", leds.update)

module.exports = router
