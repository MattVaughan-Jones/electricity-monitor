import { Line } from 'react-chartjs-2'

import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Colors,
  type TooltipItem,
} from 'chart.js'

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Colors
)

export type RecordingData = {
  fileName: string
  recordingName: string
  data: {
    type: string
    timestamp: number
    voltage: number
    current: number
    power: number
    frequency: number
  }[]
}

export type ComparisonGraphProps = {
  recordings: RecordingData[]
  dataKey: 'voltage' | 'current' | 'power' | 'frequency'
  label: string
  unit: string
}

export const ComparisonGraph = ({
  recordings,
  dataKey,
  label,
  unit,
}: ComparisonGraphProps) => {
  if (recordings.length === 0) {
    return <></>
  }

  // Find the recording with the most data points (longest duration)
  const longestRecording = recordings.reduce((longest, current) =>
    current.data.length > longest.data.length ? current : longest
  )

  // Use the longest recording's time scale
  const timeLabels = longestRecording.data.map(row =>
    (row.timestamp / 1000).toFixed(1)
  )

  const comparisonColors = [
    'rgb(255, 159, 64)',
    'rgb(153, 102, 255)',
    'rgb(255, 99, 255)',
    'rgb(99, 255, 132)',
    'rgb(255, 255, 99)',
    'rgb(255, 99, 132)',
    'rgb(54, 162, 235)',
    'rgb(75, 192, 192)',
  ]

  const datasets = recordings.map((recording, index) => ({
    label: `${recording.recordingName} - ${label} (${unit})`,
    data: recording.data.map(row => row[dataKey]),
    borderColor: comparisonColors[index % comparisonColors.length],
    backgroundColor: comparisonColors[index % comparisonColors.length]
      .replace('rgb', 'rgba')
      .replace(')', ', 0.2)'),
    yAxisID: 'y',
    tension: 0.1,
  }))

  const data = {
    labels: timeLabels,
    datasets,
  }

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    interaction: {
      mode: 'index' as const,
      intersect: false,
    },
    scales: {
      x: {
        display: true,
        grid: {
          color: 'rgba(255, 255, 255, 0.1)',
        },
        ticks: {
          color: 'white',
        },
        title: {
          display: true,
          text: 'Time (seconds)',
          color: 'white',
        },
      },
      y: {
        type: 'linear' as const,
        display: true,
        position: 'left' as const,
        grid: {
          color: 'rgba(255, 255, 255, 0.1)',
        },
        min: 0,
        ticks: {
          color: 'white',
        },
        title: {
          display: true,
          text: `${label} (${unit})`,
          color: 'white',
        },
      },
    },
    plugins: {
      legend: {
        position: 'top' as const,
        labels: {
          color: 'white',
          usePointStyle: true,
        },
      },
      tooltip: {
        callbacks: {
          label: function (context: TooltipItem<'line'>) {
            const label = context.dataset.label || ''
            const value = context.parsed.y
            return `${label}: ${value.toFixed(2)}`
          },
        },
      },
    },
  }

  return (
    <div style={{ height: '25vh' }}>
      <Line data={data} options={options} />
    </div>
  )
}
