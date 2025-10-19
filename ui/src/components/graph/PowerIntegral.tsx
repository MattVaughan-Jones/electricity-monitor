import {
  Box,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
} from '@mui/material'
import { Bar } from 'react-chartjs-2'
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  LogarithmicScale,
  BarElement,
  Title,
  Tooltip,
  Legend,
} from 'chart.js'
import { type RecordingData } from './Graph'

ChartJS.register(
  CategoryScale,
  LinearScale,
  LogarithmicScale,
  BarElement,
  Title,
  Tooltip,
  Legend
)

export type PowerIntegralProps = {
  recordings: RecordingData[]
}

export const PowerIntegral = ({ recordings }: PowerIntegralProps) => {
  if (recordings.length === 0) {
    return (
      <Box sx={{ p: 2, textAlign: 'center' }}>
        <Typography variant="body2" color="text.secondary">
          No recordings selected
        </Typography>
      </Box>
    )
  }

  const calculatePowerIntegral = (data: RecordingData['data']): number => {
    if (data.length < 2) return 0

    let integral = 0
    for (let i = 1; i < data.length; i++) {
      const timeDiff = (data[i].timestamp - data[i - 1].timestamp) / 1000 // Convert to seconds
      const avgPower = (data[i].power + data[i - 1].power) / 2 // Average power between points
      integral += avgPower * timeDiff // Power * time = energy
    }

    return integral / 3600 // Convert from Ws to Wh
  }

  const formatDuration = (data: RecordingData['data']): string => {
    if (data.length < 2) return '0s'

    const durationMs = data[data.length - 1].timestamp - data[0].timestamp
    const durationSeconds = durationMs / 1000

    if (durationSeconds < 60) {
      return `${durationSeconds.toFixed(1)}s`
    } else if (durationSeconds < 3600) {
      const minutes = Math.floor(durationSeconds / 60)
      const seconds = durationSeconds % 60
      return `${minutes}m ${seconds.toFixed(1)}s`
    } else {
      const hours = Math.floor(durationSeconds / 3600)
      const minutes = Math.floor((durationSeconds % 3600) / 60)
      return `${hours}h ${minutes}m`
    }
  }

  const recordingSummaries = recordings
    .map(recording => ({
      name: recording.recordingName,
      duration: formatDuration(recording.data),
      durationSeconds:
        recording.data.length > 1
          ? (
              (recording.data[recording.data.length - 1].timestamp -
                recording.data[0].timestamp) /
              1000
            ).toFixed(1)
          : '0.0',
      energyWh: calculatePowerIntegral(recording.data),
    }))
    .sort((a, b) => b.energyWh - a.energyWh) // Sort by energy consumption (highest first)

  return (
    <Box
      sx={{
        p: 1,
        height: '100%',
        overflow: 'hidden',
        display: 'flex',
        flexDirection: 'column',
      }}
    >
      <Box sx={{ flex: 1, display: 'flex', gap: 1 }}>
        {/* Table */}
        <Box sx={{ flex: 0.5, height: '160px', overflow: 'hidden' }}>
          <TableContainer
            sx={{
              backgroundColor: 'transparent',
              height: '160px',
              overflow: 'auto',
              '& .MuiTableCell-root': {
                borderColor: 'rgba(255, 255, 255, 0.1)',
                color: 'white',
                fontSize: '0.8rem',
                padding: '2px 4px',
              },
              '& .MuiTableHead-root .MuiTableCell-root': {
                backgroundColor: 'rgba(255, 255, 255, 0.1)',
                fontWeight: 'bold',
                fontSize: '0.8rem',
              },
            }}
          >
            <Table size="small">
              <TableHead>
                <TableRow>
                  <TableCell>Device</TableCell>
                  <TableCell align="right">Duration (s)</TableCell>
                  <TableCell align="right">Wh</TableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                {recordingSummaries.map((summary, index) => (
                  <TableRow key={index}>
                    <TableCell
                      sx={{
                        maxWidth: '100px',
                        overflow: 'hidden',
                        textOverflow: 'ellipsis',
                        whiteSpace: 'nowrap',
                      }}
                    >
                      {summary.name}
                    </TableCell>
                    <TableCell align="right">
                      {summary.durationSeconds}
                    </TableCell>
                    <TableCell align="right">
                      {summary.energyWh.toFixed(4)}
                    </TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>
        </Box>

        {/* Bar Graph */}
        <Box sx={{ flex: 0.5, height: '160px' }}>
          <div style={{ height: '160px' }}>
            <Bar
              data={{
                labels: recordingSummaries.map(s => s.name),
                datasets: [
                  {
                    label: 'Energy (Wh)',
                    data: recordingSummaries.map(s => s.energyWh),
                    backgroundColor: [
                      'rgba(255, 99, 132, 0.8)',
                      'rgba(54, 162, 235, 0.8)',
                      'rgba(255, 205, 86, 0.8)',
                      'rgba(75, 192, 192, 0.8)',
                      'rgba(153, 102, 255, 0.8)',
                    ],
                    borderColor: [
                      'rgba(255, 99, 132, 1)',
                      'rgba(54, 162, 235, 1)',
                      'rgba(255, 205, 86, 1)',
                      'rgba(75, 192, 192, 1)',
                      'rgba(153, 102, 255, 1)',
                    ],
                    borderWidth: 1,
                  },
                ],
              }}
              options={{
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                  legend: {
                    display: false,
                  },
                  tooltip: {
                    callbacks: {
                      label: function (context) {
                        return `Energy: ${context.parsed.y.toFixed(4)} Wh`
                      },
                    },
                  },
                },
                scales: {
                  x: {
                    ticks: {
                      color: 'white',
                      font: { size: 10 },
                      maxRotation: 45,
                    },
                    grid: {
                      color: 'rgba(255, 255, 255, 0.1)',
                    },
                  },
                  y: {
                    type: 'logarithmic',
                    ticks: {
                      color: 'white',
                      font: { size: 10 },
                    },
                    grid: {
                      color: 'rgba(255, 255, 255, 0.1)',
                    },
                    title: {
                      display: true,
                      text: 'Energy (Wh)',
                      color: 'white',
                      font: { size: 12 },
                    },
                  },
                },
              }}
            />
          </div>
        </Box>
      </Box>
    </Box>
  )
}
