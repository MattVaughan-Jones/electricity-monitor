import { useState } from 'react'
import './StartStop.css'
import { Snackbar } from '@mui/material'

export const StartStop = () => {
  const [startSnackbarOpen, setStartSnackbarOpen] = useState(false)
  const [stopSnackbarOpen, setStopSnackbarOpen] = useState(false)

  const onClickStart = async () => {
    const res = await fetch('http://localhost:8080/start-recording')
    if (res.ok) {
      setStartSnackbarOpen(true)
    }
  }

  const onClickStop = async () => {
    const res = await fetch('http://localhost:8080/stop-recording')
    if (res.ok) {
      setStopSnackbarOpen(true)
    }
  }

  const closeStartSnackbar = () => {
    setStartSnackbarOpen(false)
  }
  const closeStopSnackbar = () => {
    setStopSnackbarOpen(false)
  }

  return (
    <>
      <button onClick={onClickStart}>Start</button>
      <Snackbar
        open={startSnackbarOpen}
        autoHideDuration={3000}
        message="Recording started"
        onClose={closeStartSnackbar}
      />
      <button onClick={onClickStop}>Stop</button>
      <Snackbar
        open={stopSnackbarOpen}
        autoHideDuration={3000}
        message="Recording stopped"
        onClose={closeStopSnackbar}
      />
    </>
  )
}
