import { Box, Button, Card } from '@mui/material'

type RecordingsListItem = {
  name: string
  datetime: Date
  fileName: string
}

export const LeftMenu = ({
  recordings,
  onSelectRecording,
  onRefresh,
}: {
  recordings: string[]
  onSelectRecording: (fileName: string) => void
  onRefresh: () => void
}) => {
  const parseRecordingName = (fileName: string): RecordingsListItem => {
    // Try to parse from fileName like "rec4_2025-10-10T08.20.08.json"
    const match = fileName.match(
      /^(.+)_(\d{4}-\d{2}-\d{2}T\d{2}\.\d{2}\.\d{2})\.json$/
    )
    if (match && match[1] && match[2]) {
      // Convert 2025-10-10T08.20.08 -> 2025-10-10T08:20:08 for Date
      const iso = match[2].replace(/\./g, ':')
      const datetime = new Date(iso)
      if (!isNaN(datetime.getTime())) {
        return { name: match[1], datetime, fileName }
      }
    }

    // Fallback: use filename without extension as name, current time as datetime
    return {
      name: fileName.replace('.json', ''),
      datetime: new Date(),
      fileName,
    }
  }

  const sortRecordings = (
    recordings: RecordingsListItem[]
  ): RecordingsListItem[] => {
    return [...recordings].sort((a, b) => {
      return a.datetime.getTime() - b.datetime.getTime()
    })
  }

  const parsedRecordings = recordings.map(recording =>
    parseRecordingName(recording)
  )
  const sortedRecordings = sortRecordings(parsedRecordings)

  return (
    <Box
      sx={{ width: '100%', maxWidth: '100%', p: 2, boxSizing: 'border-box' }}
    >
      <Box
        sx={{
          display: 'flex',
          justifyContent: 'space-between',
          alignItems: 'center',
          mb: 2,
        }}
      >
        <p
          style={{
            color: 'white',
            fontSize: '1.2em',
            fontWeight: 'bold',
            margin: 0,
          }}
        >
          Recordings
        </p>
        <Button
          variant="outlined"
          size="small"
          onClick={onRefresh}
          sx={{ color: 'white', borderColor: 'white' }}
        >
          Refresh
        </Button>
      </Box>
      <div style={{ width: '100%' }}>
        {sortedRecordings.map(recording => {
          return (
            <Card
              key={recording.fileName}
              sx={{
                width: '100%',
                p: 1.5,
                mb: 1.5,
                boxSizing: 'border-box',
                bgcolor: '#2a2a2a',
                cursor: 'pointer',
              }}
              onClick={() => onSelectRecording(recording.fileName)}
            >
              <div style={{ fontWeight: 600, color: '#fff' }}>
                {recording.name}
              </div>
              <div style={{ color: '#bbb', fontStyle: 'italic', fontSize: 12 }}>
                {recording.datetime.toDateString()}
              </div>
            </Card>
          )
        })}
      </div>
    </Box>
  )
}
