import { useState, useEffect } from 'react'
import { Graph, type RecordingData } from './graph/Graph'
import { ComparisonGraph } from './graph/ComparisonGraph'
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
  recording: RecordingDataFromServer,
  fileName: string
): RecordingData => {
  const { recording_name, ...rest } = recording
  return {
    ...rest,
    fileName,
    recordingName: recording_name,
  }
}

export const Recording = () => {
  const [recordings, setRecordings] = useState<string[]>([])
  const [graphData, setGraphData] = useState<RecordingData | undefined>(
    undefined
  )
  const [selectedRecordings, setSelectedRecordings] = useState<Set<string>>(
    new Set()
  )
  const [comparisonData, setComparisonData] = useState<
    Map<string, RecordingData>
  >(new Map())

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

      const graphData = convertRecordingData(json, fileName)

      setGraphData(graphData)
    } catch (e) {
      console.error('failed to load recording', e)
      setGraphData(undefined)
    }
  }

  const onToggleRecording = async (fileName: string) => {
    const isCurrentlySelected = selectedRecordings.has(fileName)

    if (isCurrentlySelected) {
      // Remove from selection
      setSelectedRecordings(prev => {
        const newSet = new Set(prev)
        newSet.delete(fileName)
        return newSet
      })
      setComparisonData(prev => {
        const newMap = new Map(prev)
        newMap.delete(fileName)
        return newMap
      })
    } else {
      // Add to selection
      try {
        const res = await fetch(`http://localhost:8080/recording/${fileName}`)
        if (!res.ok) return
        const json: RecordingDataFromServer = await res.json()
        const recordingData = convertRecordingData(json, fileName)

        setSelectedRecordings(prev => new Set(prev).add(fileName))
        setComparisonData(prev => new Map(prev).set(fileName, recordingData))
      } catch (e) {
        console.error('failed to load recording for comparison', e)
      }
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
            selectedRecordings={Array.from(selectedRecordings)}
            onToggleRecording={onToggleRecording}
            currentRecording={graphData?.fileName}
          />
        </div>
        <Box sx={{ flex: 1 }}>
          <Box sx={{ pb: 3 }}>
            <RecordingControl onRefresh={loadRecordings} />
          </Box>

          {graphData && (
            <Box sx={{ mb: 2, textAlign: 'center' }}>
              <h2 style={{ color: 'white', margin: 0, fontSize: '1.5em' }}>
                {graphData.recordingName}
              </h2>
            </Box>
          )}

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

          {/* Power Comparison Graph - Always reserve space */}
          <Box sx={{ mt: 3, height: '25vh' }}>
            {comparisonData.size > 0 && (
              <ComparisonGraph
                recordings={Array.from(comparisonData.values())}
                dataKey="power"
                label="Power"
                unit="W"
              />
            )}
          </Box>
        </Box>
      </div>
    </>
  )
}
