import { powerData } from '../fixtures/power-data'
import { Graph } from './graph/Graph'
import { LeftMenu } from './LeftMenu'

export const Recording = () => {
  return (
    <div style={{ display: 'flex' }}>
      <div style={{ width: '20%' }}>
        <LeftMenu />
      </div>
      <div style={{ width: '80%' }}>
        <Graph powerData={powerData} />
      </div>
    </div>
  )
}
