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
}

export const Graph = ({
  inputData,
  dataKey,
  label,
  unit,
  color,
  backgroundColor,
}: GraphProps) => {
  if (!inputData) {
    return <></>
  }

  // Convert timestamps to seconds
  const timeLabels = inputData.data.map(row =>
    (row.timestamp / 1000).toFixed(1)
  )

  const data = {
    labels: timeLabels,
    datasets: [
      {
        label: `${label} (${unit})`,
        data: inputData.data.map(row => row[dataKey]),
        borderColor: color,
        backgroundColor: backgroundColor,
        yAxisID: 'y',
        tension: 0.1,
      },
    ],
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
        display: false,
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
    <div style={{ height: '100%' }}>
      <Line data={data} options={options} />
    </div>
  )
}
