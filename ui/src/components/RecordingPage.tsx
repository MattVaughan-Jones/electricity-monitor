import { useState, useEffect } from 'react'
import { Graph, type RecordingData } from './graph/Graph'
import { LeftMenu } from './LeftMenu'
import { RecordingControl } from './RecordingControl'
import { Box } from '@mui/material'

type RecordingDataFromServer = {
  fileName: string
  recording_name: string
  data: {
    type: string
    timestamp: number
    voltage: number
    current: number
    power: number
    frequency: number
  }[]
}

const convertRecordingData = (
  recording: RecordingDataFromServer
): RecordingData => {
  const { recording_name, ...rest } = recording
  return {
    ...rest,
    recordingName: recording_name,
  }
}

export const Recording = () => {
  const [recordings, setRecordings] = useState<string[]>([])
  const [graphData, setGraphData] = useState<RecordingData | undefined>(
    undefined
  )

  const loadRecordings = async () => {
    try {
      const res = await fetch('http://localhost:8080/recordings')
      if (!res.ok) return
      const fileNames: string[] = await res.json()

      setRecordings(fileNames)
    } catch (e) {
      console.error('failed to load recordings', e)
    }
  }

  const onSelectRecording = async (fileName: string) => {
    try {
      const res = await fetch(`http://localhost:8080/recording/${fileName}`)
      if (!res.ok) return
      const json: RecordingDataFromServer = await res.json()
      console.log(json)

      const graphData = convertRecordingData(json)

      setGraphData(graphData)
    } catch (e) {
      console.error('failed to load recording', e)
      setGraphData(undefined)
    }
  }

  useEffect(() => {
    loadRecordings()
  }, [])

  return (
    <>
      <div style={{ display: 'flex', gap: '50px' }}>
        <div style={{ width: '300px', minWidth: '300px', maxWidth: '300px' }}>
          <LeftMenu
            recordings={recordings}
            onSelectRecording={onSelectRecording}
            onRefresh={loadRecordings}
          />
        </div>
        <Box sx={{ flex: 1 }}>
          <Box sx={{ pb: 3 }}>
            <RecordingControl />
          </Box>
          <Box
            sx={{
              display: 'grid',
              gridTemplateColumns: '1fr 1fr',
              gridTemplateRows: '1fr 1fr',
              gap: 2,
              height: '50vh',
              mb: 5,
            }}
          >
            <Graph
              inputData={graphData}
              dataKey="voltage"
              label="Voltage"
              unit="V"
              color="rgb(255, 99, 132)"
              backgroundColor="rgba(255, 99, 132, 0.2)"
            />
            <Graph
              inputData={graphData}
              dataKey="current"
              label="Current"
              unit="A"
              color="rgb(54, 162, 235)"
              backgroundColor="rgba(54, 162, 235, 0.2)"
            />
            <Graph
              inputData={graphData}
              dataKey="power"
              label="Power"
              unit="W"
              color="rgb(75, 192, 192)"
              backgroundColor="rgba(75, 192, 192, 0.2)"
            />
            <Graph
              inputData={graphData}
              dataKey="frequency"
              label="Frequency"
              unit="Hz"
              color="rgb(255, 205, 86)"
              backgroundColor="rgba(255, 205, 86, 0.2)"
            />
          </Box>
        </Box>
      </div>
    </>
  )
}
