import { useState } from 'react'
// import './RecordingControl.css'
import { Snackbar, TextField } from '@mui/material'

export const RecordingControl = () => {
  const [startSnackbarOpen, setStartSnackbarOpen] = useState(false)
  const [stopSnackbarOpen, setStopSnackbarOpen] = useState(false)
  const [recordingName, setRecordingName] = useState('')

  const onClickStart = async () => {
    const res = await fetch('http://localhost:8080/start-recording', {
      method: 'POST',
      body: JSON.stringify({ recordingName }),
    })
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
      <TextField
        id="recording-name"
        label="Recording Name"
        variant="filled"
        onChange={e => setRecordingName(e.target.value)}
      />
      <div>
        <button onClick={onClickStart} style={{ margin: '10px' }}>
          Start
        </button>
        <Snackbar
          open={startSnackbarOpen}
          autoHideDuration={3000}
          message="Recording started"
          onClose={closeStartSnackbar}
        />
        <button onClick={onClickStop} style={{ margin: '10px' }}>
          Stop
        </button>
        <Snackbar
          open={stopSnackbarOpen}
          autoHideDuration={3000}
          message="Recording stopped"
          onClose={closeStopSnackbar}
        />
      </div>
    </>
  )
}
