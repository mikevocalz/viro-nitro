import React, { useState } from 'react'
import { StyleSheet, View, Button, Text, PermissionsAndroid, Platform, Alert } from 'react-native'
import {
  KimoyoOjuView,
  KimoyoOjuScene,
  KimoyoOjuNode,
  KimoyoOjuBox,
  KimoyoOjuSphere,
  KimoyoOjuLight,
  KimoyoOjuCamera,
  KimoyoOjuText,
  useKimoyoOjuEngine,
} from '@kimoyo-oju/react'

function Scene() {
  return (
    <KimoyoOjuScene>
      {/* Ambient light for base illumination */}
      <KimoyoOjuLight type="ambient" color={[0.3, 0.3, 0.3, 1]} />
      
      {/* Directional light for shadows */}
      <KimoyoOjuLight 
        type="directional" 
        color={[1, 1, 1, 1]} 
        intensity={0.8}
        rotation={[0.5, -0.5, 0, 0.7]}
      />
      
      {/* Camera for flat mode */}
      <KimoyoOjuCamera position={[0, 0, 0]} />
      
      {/* Red box */}
      <KimoyoOjuBox
        position={[-1, 0, -5]}
        width={1}
        height={1}
        length={1}
        color={[1, 0.2, 0.2, 1]}
      />
      
      {/* Blue sphere */}
      <KimoyoOjuSphere
        position={[1, 0, -5]}
        radius={0.5}
        color={[0.2, 0.2, 1, 1]}
      />
      
      {/* Text label */}
      <KimoyoOjuText
        text="Kimoyo Oju"
        position={[0, 1.5, -5]}
        fontSize={24}
        color={[1, 1, 1, 1]}
      />
      
      {/* Nested group with multiple objects */}
      <KimoyoOjuNode position={[0, -1, -5]}>
        <KimoyoOjuBox
          position={[-0.5, 0, 0]}
          width={0.3}
          height={0.3}
          length={0.3}
          color={[0.2, 1, 0.2, 1]}
        />
        <KimoyoOjuBox
          position={[0.5, 0, 0]}
          width={0.3}
          height={0.3}
          length={0.3}
          color={[1, 1, 0.2, 1]}
        />
      </KimoyoOjuNode>
    </KimoyoOjuScene>
  )
}

interface ControlsProps {
  onModeChange: (mode: 'flat' | 'immersive-vr' | 'immersive-mr') => void
}

function Controls({ onModeChange }: ControlsProps) {
  const { getState, getMode, enterXR, exitXR, getMemoryCounters } = useKimoyoOjuEngine()
  const [status, setStatus] = useState('')

  const handleEnterVR = () => {
    const success = enterXR('immersive-vr')
    if (success) {
      onModeChange('immersive-vr')
      setStatus('Entered VR')
    } else {
      setStatus('Failed to enter VR')
    }
  }

  const handleEnterAR = async () => {
    if (Platform.OS === 'android') {
      try {
        const granted = await PermissionsAndroid.request(
          PermissionsAndroid.PERMISSIONS.CAMERA,
          {
            title: 'Camera Permission',
            message: 'AR mode needs camera access to show the real world',
            buttonNeutral: 'Ask Later',
            buttonNegative: 'Cancel',
            buttonPositive: 'OK',
          }
        )
        if (granted !== PermissionsAndroid.RESULTS.GRANTED) {
          setStatus('Camera permission denied')
          return
        }
      } catch (err) {
        console.warn(err)
        setStatus('Error requesting camera permission')
        return
      }
    }
    const success = enterXR('immersive-mr')
    if (success) {
      onModeChange('immersive-mr')
      setStatus('AR Mode - See real world!')
    } else {
      setStatus('Failed to enter AR')
    }
  }

  const handleExitVR = () => {
    const success = exitXR()
    if (success) {
      onModeChange('flat')
      setStatus('Exited XR')
    } else {
      setStatus('Failed to exit XR')
    }
  }

  const handleShowStats = () => {
    const counters = getMemoryCounters()
    setStatus(
      `Nodes: ${counters.liveNodes}, Textures: ${counters.liveTextures}, ` +
      `GPU: ${(counters.gpuMemoryBytes / 1024 / 1024).toFixed(1)}MB`
    )
  }

  return (
    <View style={styles.controls}>
      <Text style={styles.status}>
        State: {getState()} | Mode: {getMode()}
      </Text>
      <Text style={styles.status}>{status}</Text>
      <View style={styles.buttons}>
        <Button title="AR" onPress={handleEnterAR} />
        <Button title="VR" onPress={handleEnterVR} />
        <Button title="Exit" onPress={handleExitVR} />
        <Button title="Stats" onPress={handleShowStats} />
      </View>
    </View>
  )
}

export default function App() {
  const [xrMode, setXrMode] = useState<'flat' | 'immersive-vr' | 'immersive-mr'>('flat')
  
  const handleError = (code: string, message: string) => {
    console.error(`Kimoyo Oju Error [${code}]: ${message}`)
  }

  return (
    <View style={styles.container}>
      <KimoyoOjuView 
        style={styles.kimoyoOjuView} 
        mode={xrMode}
        onError={handleError}
      >
        <Scene />
      </KimoyoOjuView>
      <Controls onModeChange={setXrMode} />
    </View>
  )
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#000',
  },
  kimoyoOjuView: {
    flex: 1,
  },
  controls: {
    position: 'absolute',
    bottom: 40,
    left: 20,
    right: 20,
    backgroundColor: 'rgba(0,0,0,0.7)',
    padding: 15,
    borderRadius: 10,
  },
  status: {
    color: '#fff',
    fontSize: 12,
    marginBottom: 10,
    textAlign: 'center',
  },
  buttons: {
    flexDirection: 'row',
    justifyContent: 'space-around',
  },
})
