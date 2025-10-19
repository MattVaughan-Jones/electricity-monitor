import { useState } from 'react'
import { Snackbar, TextField, styled } from '@mui/material'

const StyledTextField = styled(TextField)(() => ({
  '& .MuiInputLabel-root': {
    color: 'white',
  },
  '& .MuiInputBase-input': {
    color: 'white',
  },
}))

export const RecordingControl = ({ onRefresh }: { onRefresh: () => void }) => {
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
      onRefresh() // Refresh the recordings list
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
      <StyledTextField
        id="recording-name"
        label="Recording Name"
        variant="filled"
        onChange={(e: React.ChangeEvent<HTMLInputElement>) =>
          setRecordingName(e.target.value)
        }
      />
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
    </>
  )
}
