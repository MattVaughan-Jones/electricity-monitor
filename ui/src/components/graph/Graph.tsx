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

export type GraphProps = {
  inputData: RecordingData | undefined
  dataKey: 'voltage' | 'current' | 'power' | 'frequency'
  label: string
  unit: string
  color: string
  backgroundColor: string
  comparisonData?: RecordingData[]
}

export const Graph = ({
  inputData,
  dataKey,
  label,
  unit,
  color,
  backgroundColor,
  comparisonData = [],
}: GraphProps) => {
  if (!inputData && comparisonData.length === 0) {
    return <></>
  }

  // Convert timestamps to seconds (timestamps are now relative milliseconds from ESP32)
  const timeLabels =
    inputData?.data.map(row => (row.timestamp / 1000).toFixed(1)) || []

  // Generate datasets
  const datasets = []

  // Add main dataset if inputData exists
  if (inputData) {
    datasets.push({
      label: `${label} (${unit})`,
      data: inputData.data.map(row => row[dataKey]),
      borderColor: color,
      backgroundColor: backgroundColor,
      yAxisID: 'y',
      tension: 0.1,
    })
  }

  // Add comparison datasets if this is a power graph and comparison data exists
  if (dataKey === 'power' && comparisonData.length > 0) {
    const comparisonColors = [
      'rgb(255, 159, 64)', // Orange
      'rgb(153, 102, 255)', // Purple
      'rgb(255, 99, 255)', // Pink
      'rgb(99, 255, 132)', // Green
      'rgb(255, 255, 99)', // Yellow
    ]

    comparisonData.forEach((compData, index) => {
      datasets.push({
        label: `${compData.recordingName} - Power (W)`,
        data: compData.data.map(row => row.power),
        borderColor: comparisonColors[index % comparisonColors.length],
        backgroundColor: comparisonColors[index % comparisonColors.length]
          .replace('rgb', 'rgba')
          .replace(')', ', 0.2)'),
        yAxisID: 'y',
        tension: 0.1,
      })
    })

    // If no main inputData, create time labels based on the longest recording
    if (!inputData && comparisonData.length > 0) {
      // Find the recording with the most data points (longest duration)
      const longestRecording = comparisonData.reduce((longest, current) =>
        current.data.length > longest.data.length ? current : longest
      )

      // For comparison graphs, use the longest recording's time scale
      // Timestamps are now relative milliseconds from ESP32, so just convert to seconds
      timeLabels.length = 0
      timeLabels.push(
        ...longestRecording.data.map(
          row => (row.timestamp / 1000).toFixed(1) // Convert relative ms to seconds
        )
      )
    }
  }

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
        ticks: {
          color: color,
        },
        title: {
          display: true,
          text: `${label} (${unit})`,
          color: color,
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
