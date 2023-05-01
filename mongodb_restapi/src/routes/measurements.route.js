const express = require("express")
const router = express.Router()
const measurements = require("../controllers/measurements.controller")

router.get("/:chipId", measurements.getAll)

router.get("/:chipId/:startDate/:stopDate", measurements.getBetweenDate)

router.post("/:chipId", measurements.create)

module.exports = router
