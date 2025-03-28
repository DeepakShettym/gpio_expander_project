package com.example

import android.annotation.SuppressLint
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import android.widget.Toast
import com.example.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    @OptIn(ExperimentalStdlibApi::class)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        callJavaFunction()

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

        binding.turnLedOn.setOnClickListener {
            ledDriver_Init()
            initJVM()
        }

        binding.turnLedOff.setOnClickListener {
            ledDriver_DeInit()
        }

        binding.turnLedDriveOn.setOnClickListener {
            ledDriver_Output(0x01)
        }

        binding.turnLedDriveOff.setOnClickListener {
            ledDriver_Output(0x00)
        }

        binding.ledSetIntensity.setOnClickListener {
            Log.d("LED_CONTROL", "onCreate1: ${binding.setLedIntensityValue.text.toString()}")
            if(binding.setLedIntensityValue.text.toString().length > 0){
                try {
                    val ledBrightnessValueSet= binding.setLedIntensityValue.text.toString().toInt(16).toByte()
                    Log.d("LED_CONTROL ledSetIntensity", "onCreate2: ${ledBrightnessValueSet}")
                    ledDriver_SetIntensity(ledBrightnessValueSet)
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledGetIntensity.setOnClickListener {
           binding.getLedIntensityValue.text = ledDriver_GetIntensity().toHexString().toString()
        }

        binding.ledGetOutputStatus.setOnClickListener {
            binding.getLedStatusValue.hint = ledDriver_GetOutputStatus().toString()
        }

        binding.ledSetIntensityC1.setOnClickListener {
            Log.d("ledDriver_SetChannelTonTime", "onCreate1: ${binding.setLedIntensityValueC1.text.toString()}")
            if(binding.setLedIntensityValueC1.text.toString().length > 0){
                try {
                    val ledBrightnessValueSet= binding.setLedIntensityValueC1.text.toString().toInt(16)
                    Log.d("ledDriver_SetChannelTonTime", "onCreate2: ${ledBrightnessValueSet}")
                    ledDriver_SetChannelTonTime(1.toByte(),ledBrightnessValueSet )
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledSetIntensityC2.setOnClickListener {
            Log.d("ledDriver_SetChannelTonTime", "onCreate1: ${binding.setLedIntensityValueC2.text.toString()}")
            if(binding.setLedIntensityValueC2.text.toString().length > 0){
                try {
                    val ledBrightnessValueSet= binding.setLedIntensityValueC2.text.toString().toInt(16)
                    Log.d("ledDriver_SetChannelTonTime", "onCreate2: ${ledBrightnessValueSet}")
                    ledDriver_SetChannelTonTime(2.toByte(),ledBrightnessValueSet)
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledSetIntensityC3.setOnClickListener {
            Log.d("ledDriver_SetChannelTonTime", "onCreate1: ${binding.setLedIntensityValueC3.text.toString()}")
            if(binding.setLedIntensityValueC3.text.toString().length > 0){
                try {
                    val ledBrightnessValueSet= binding.setLedIntensityValueC3.text.toString().toInt(16)
                    Log.d("ledDriver_SetChannelTonTime", "onCreate2: ${ledBrightnessValueSet}")
                    ledDriver_SetChannelTonTime(3.toByte(),ledBrightnessValueSet)
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        
        binding.ledSetIntensityC4.setOnClickListener {
            Log.d("ledDriver_SetChannelTonTime", "onCreate1: ${binding.setLedIntensityValueC4.text.toString()}")
            if(binding.setLedIntensityValueC4.text.toString().length > 0){
                try {
                    val ledBrightnessValueSet= binding.setLedIntensityValueC4.text.toString().toInt(16)
                    Log.d("ledDriver_SetChannelTonTime", "onCreate2: ${ledBrightnessValueSet}")
                    ledDriver_SetChannelTonTime(4.toByte(),ledBrightnessValueSet)
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledGetIntensityc1.setOnClickListener {
            binding.getLedIntensityValuec1.text = ledDriver_GetChannelTonTime(1.toByte()).toHexString().toString()
        }

        binding.ledGetIntensityc2.setOnClickListener {
            binding.getLedIntensityValuec2.text = ledDriver_GetChannelTonTime(2.toByte()).toHexString().toString()
        }

        binding.ledGetIntensityc3.setOnClickListener {
            binding.getLedIntensityValuec3.text = ledDriver_GetChannelTonTime(3.toByte()).toHexString().toString()
        }

        binding.ledGetIntensityc4.setOnClickListener {
            binding.getLedIntensityValuec4.text = ledDriver_GetChannelTonTime(4.toByte()).toHexString().toString()
        }

        binding.ledSetChannel.setOnClickListener{
            if(binding.setLedChannelNumber.text.toString().isNotEmpty() && binding.setLedChannelNumberValue.text.toString().isNotEmpty()){
                try {
                    val ledChannelNumber= binding.setLedChannelNumber.text.toString().toInt(16)
                    val ledChannelValue = binding.setLedChannelNumberValue.text.toString().toInt(16)
                    Log.d("ledSetChannel", "onCreate2: ${ledChannelNumber}")
                    ledDriver_EnableChannel(ledChannelNumber.toByte(),ledChannelValue.toByte())
                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledGetChannel.setOnClickListener {
            if(binding.setLedChannelStatus.text.toString().length > 0){
                try {

                    val ledChannelNumber= binding.setLedChannelStatus.text.toString().toInt(16)
                    binding.setLedChannelStatusValue.text = buildString {
                        append("Val:")
                        append(ledDriver_GetChannelStatus(ledChannelNumber.toByte()).toString())
                    }

                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledGetRegister.setOnClickListener {
            if(binding.getLedRegisterStatus.text.toString().length > 0){
                try {

                    val ledChannelNumber= binding.getLedRegisterStatus.text.toString().toInt(16).toByte()
                    Log.d("ledGetRegister", "onCreate2: ${ledChannelNumber}")
                    binding.getLedRegisterStatusValue.text = buildString {
                        append(":")
                        append(getLedRegisterStatusValue(ledChannelNumber).toString())
                    }


                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.ledSetRegister.setOnClickListener {
            if((binding.setLedRegisterAddress.text.toString().length > 0) && (binding.setLedRegisterStatusValue.text.toString().length > 0)){
                try {

                    val ledRegisterNumber= binding.setLedRegisterAddress.text.toString().toInt(16).toByte()
                    val ledRegisterValue= binding.setLedRegisterStatusValue.text.toString().toInt(16).toByte()
                    setLedRegisterValue(ledRegisterNumber,ledRegisterValue)
                    Log.d("ledRegisterValue", "onCreate2: ${ledRegisterNumber}  ${ledRegisterValue}")



                } catch (e: Exception){
                    Toast.makeText(this@MainActivity, "Error ${e}", Toast.LENGTH_LONG).show()
                }
            }
        }

        binding.sysInit.setOnClickListener{
            sysInit()
        }

        binding.sysDinit.setOnClickListener {
            sysDinit()
        }

        binding.solenoid1TurnOn.setOnClickListener {
            solenoidSet(10,1)
        }

        binding.solenoid1TurnOff.setOnClickListener {
            solenoidSet(10,0)
        }

        binding.pwmTurnOn.setOnClickListener {
            turn_on_pwm_()
        }

        binding.pwmTurnOff.setOnClickListener {
            turn_off_pwm_()
        }

        binding.rgbLedChOn.setOnClickListener {
            ledSetFrontPanel(0x00, 0x00)
        }

        binding.rgbLedChOff.setOnClickListener {
            ledSetFrontPanel(0xff, 0xff)
        }

        binding.jvmInit.setOnClickListener {
            initJVM()
        }
    }

    /**
     * A native method that is implemented by the 'example' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun ledDriver_Init(): Int
    external fun ledDriver_Output(enable : Byte): Int
    external fun ledDriver_GetOutputStatus(): Int
    external fun ledDriver_SetIntensity(intensity: Byte): Int
    external fun ledDriver_GetIntensity(): Int
    external fun ledDriver_DeInit(): Int
    external fun ledDriver_SetChannelTonTime(channelNo: Byte, tOnTime: Int): Int
    external fun ledDriver_GetChannelTonTime(channelNo: Byte): Int
    external fun callJavaFunction()
    external fun initJVM()

    external fun ledDriver_EnableChannel(channelNo: Byte, channelEnable: Byte): Int
    external fun ledDriver_GetChannelStatus(channelNo: Byte): Int
    external fun getLedRegisterStatusValue(registerAddress: Byte): Int
    external fun setLedRegisterValue(registerAddress: Byte, registerValue: Byte): Int

    external fun sysInit(): Int
    external fun sysDinit(): Int

    external fun solenoidSet(solenoid: Int, solenoid_state: Int): Int

    external fun ledSetFrontPanel(led_front_panel: Int, led_front_panel_state: Int): Int

    external fun turn_on_pwm_(): Int
    external fun turn_off_pwm_(): Int



    fun stringToHex(input: String): String {
        require(input.length == 2) { "Input must be exactly 2 characters long" }

        val hexString = input.toCharArray().joinToString("") {
            it.code.toString(16).padStart(2, '0')
        }

        return hexString
    }
    companion object {
        // Used to load the 'example' library on application startup.
        init {
            System.loadLibrary("example")
        }
    }
}