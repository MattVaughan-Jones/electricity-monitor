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

export const Graph = ({
  inputData,
}: {
  inputData: RecordingData | undefined
}) => {
  if (!inputData) {
    return <></>
  }

  // Convert timestamps to relative time in seconds for better readability
  const startTime = inputData.data[0]?.timestamp || 0
  const timeLabels = inputData.data.map(row =>
    ((row.timestamp - startTime) / 1000).toFixed(1)
  )

  const data = {
    labels: timeLabels,
    datasets: [
      {
        label: 'Voltage (V)',
        data: inputData.data.map(row => row.voltage),
        borderColor: 'rgb(255, 99, 132)',
        backgroundColor: 'rgba(255, 99, 132, 0.2)',
        yAxisID: 'y',
        tension: 0.1,
      },
      {
        label: 'Current (A)',
        data: inputData.data.map(row => row.current),
        borderColor: 'rgb(54, 162, 235)',
        backgroundColor: 'rgba(54, 162, 235, 0.2)',
        yAxisID: 'y1',
        tension: 0.1,
      },
      {
        label: 'Power (W)',
        data: inputData.data.map(row => row.power),
        borderColor: 'rgb(75, 192, 192)',
        backgroundColor: 'rgba(75, 192, 192, 0.2)',
        yAxisID: 'y2',
        tension: 0.1,
      },
      {
        label: 'Frequency (Hz)',
        data: inputData.data.map(row => row.frequency),
        borderColor: 'rgb(255, 205, 86)',
        backgroundColor: 'rgba(255, 205, 86, 0.2)',
        yAxisID: 'y3',
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
        min: 0,
        ticks: {
          color: 'rgb(255, 99, 132)',
        },
        title: {
          display: true,
          text: 'Voltage (V)',
          color: 'rgb(255, 99, 132)',
        },
      },
      y1: {
        type: 'linear' as const,
        display: true,
        position: 'right' as const,
        grid: {
          drawOnChartArea: false,
        },
        min: 0,
        ticks: {
          color: 'rgb(54, 162, 235)',
        },
        title: {
          display: true,
          text: 'Current (A)',
          color: 'rgb(54, 162, 235)',
        },
      },
      y2: {
        type: 'linear' as const,
        display: true,
        position: 'left' as const,
        grid: {
          drawOnChartArea: false,
        },
        min: 0,
        ticks: {
          color: 'rgb(75, 192, 192)',
        },
        title: {
          display: true,
          text: 'Power (W)',
          color: 'rgb(75, 192, 192)',
        },
      },
      y3: {
        type: 'linear' as const,
        display: true,
        position: 'right' as const,
        grid: {
          drawOnChartArea: false,
        },
        min: 0,
        ticks: {
          color: 'rgb(255, 205, 86)',
        },
        title: {
          display: true,
          text: 'Frequency (Hz)',
          color: 'rgb(255, 205, 86)',
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
    <div style={{ height: '50vh' }}>
      <Line data={data} options={options} />
    </div>
  )
}
