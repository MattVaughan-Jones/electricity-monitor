import { Box, Button, Card, Checkbox, FormControlLabel } from '@mui/material'

type RecordingsListItem = {
  name: string
  datetime: Date
  fileName: string
}

export const LeftMenu = ({
  recordings,
  onSelectRecording,
  onRefresh,
  selectedRecordings,
  onToggleRecording,
  currentRecording,
}: {
  recordings: string[]
  onSelectRecording: (fileName: string) => void
  onRefresh: () => void
  selectedRecordings: string[]
  onToggleRecording: (fileName: string) => void
  currentRecording?: string
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
      <Box
        sx={{
          width: '100%',
          height: 'calc(100vh - 80px)',
          overflowY: 'auto',
          '&::-webkit-scrollbar': {
            width: '8px',
          },
          '&::-webkit-scrollbar-track': {
            backgroundColor: 'rgba(255, 255, 255, 0.1)',
            borderRadius: '4px',
          },
          '&::-webkit-scrollbar-thumb': {
            backgroundColor: 'rgba(255, 255, 255, 0.3)',
            borderRadius: '4px',
            '&:hover': {
              backgroundColor: 'rgba(255, 255, 255, 0.5)',
            },
          },
        }}
      >
        {sortedRecordings.map(recording => {
          const isSelected = selectedRecordings.includes(recording.fileName)
          const isCurrent = currentRecording === recording.fileName
          return (
            <Card
              key={recording.fileName}
              sx={{
                width: '100%',
                p: 1.5,
                mb: 1.5,
                boxSizing: 'border-box',
                bgcolor: isCurrent ? '#4a4a4a' : '#2a2a2a',
                cursor: 'pointer',
                border: isCurrent
                  ? '3px solid #00bcd4'
                  : '3px solid transparent',
              }}
              onClick={() => onSelectRecording(recording.fileName)}
            >
              <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                <FormControlLabel
                  control={
                    <Checkbox
                      checked={isSelected}
                      onChange={e => {
                        e.stopPropagation()
                        onToggleRecording(recording.fileName)
                      }}
                      sx={{
                        color: 'white',
                        '&.Mui-checked': {
                          color: 'white',
                        },
                      }}
                    />
                  }
                  label=""
                  sx={{ m: 0 }}
                />
                <Box sx={{ flex: 1 }}>
                  <div style={{ fontWeight: 600, color: '#fff' }}>
                    {recording.name}
                  </div>
                  <div
                    style={{ color: '#bbb', fontStyle: 'italic', fontSize: 12 }}
                  >
                    {recording.datetime.toLocaleDateString()}{' '}
                    {recording.datetime.toLocaleTimeString()}
                  </div>
                </Box>
              </Box>
            </Card>
          )
        })}
      </Box>
    </Box>
  )
}
