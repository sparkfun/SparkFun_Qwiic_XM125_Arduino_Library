#pragma once

// TODO: Add includes as needed (e.g. #include <Wire.h>, #include <SPI.h>)
#include "Arduino.h"
#include <Wire.h>

#include "xm125/Inc/i2c_presence_detector.h"
#include "xm125/Inc/i2c_distance_detector.h"


class QwDevXM125
{
    public:
        /// @brief TODO
        QwDevXM125();

        /// @brief This function begins the examples/communication.
        /// @return Error code (0 no error)
        bool begin();

        // --------------------- I2C Disance Detector Functions ---------------------
        
        /// @brief This function begins the presence examples/communication.
        /// @return Error code (0 no error)
        bool distanceBegin();

        /// @brief This function returns the version number of the device 
        ///  structure: major.minor.patch
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch);

        /// @brief This function returns if there was an error from the 
        ///  protocol status register 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorError(sfe_xm125_distance_protocol_status_t *error);

        /// @brief This function returns the measure counter, the number of measurements
        ///   performed since restart. 
        /// @return Error code (0 no error)
        int32_t getDistanceMeasureCounter(uint16_t *counter);

        /// @brief This function returns the status of the device if there are any issues 
        ///  with any of the status's listed defined. 
        /// @return Error code (0 no error)
        int32_t getDistanceDetectorStatus(sfe_xm125_distance_detector_status_t *status);

        /// @brief This function returns the number of detected distances.
        /// @param distance Number of detected distances
        /// @return Error code (0 no error)
        int32_t getDistanceNumberDistances(sfe_xm125_distance_result_t *distance);

        /// @brief This function returns the indication that there might be an object 
        ///   near the start point of the measured range.
        /// @param edge Flag to determine object in range 
        /// @return Error code (0 no error)
        int32_t getDistanceNearStartEdge(sfe_xm125_distance_result_t *edge);

        /// @brief This function returns the indication of a sensor calibration needed.
        /// @param calibrate Flag to indicate calibration required 
        /// @return Error code (0 no error)
        int32_t getDistanceCalibrationNeeded(sfe_xm125_distance_result_t *calibrate);

        /// @brief This function returns if the measure command failed. 
        /// @param error Flag to indicate measure command error 
        /// @return Error code (0 no error)
        int32_t getDistanceMeasureDistanceError(sfe_xm125_distance_result_t *error);

        /// @brief This function returns the temperature in sensor during measurements
        ///   (in degree Celsius). Note that it has poor absolute accuracy and should 
        ///   only be used for relative temperature measurements. 
        /// @param temperature Relative tempertaure of device 
        /// @return Error code (0 no error)
        int32_t getDistanceTemperature(sfe_xm125_distance_result_t *temperature);

        /// @brief This function returns the distance to peak 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak0Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 1
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak1Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 2
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak2Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 3
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak3Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 4
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak4Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 5
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak5Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 6
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak6Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 7
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak7Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 8
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak8Distance(uint16_t *peak);

        /// @brief This function returns the distance to peak 9
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak9Distance(uint16_t *peak);

        /// @brief This function returns the strength of peak 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak0Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 1
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak1Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 2
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak2Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 3
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak3Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 4
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak4Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 5
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak5Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 6
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak6Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 7
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak7Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 8
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak8Strength(uint16_t *peak);

        /// @brief This function returns the strength of peak 9
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @return Error code (0 no error)
        int32_t getDistancePeak9Strength(uint16_t *peak);

        /// @brief This function returns the start of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 250
        /// @return Error code (0 no error)
        int32_t getDistanceStart(uint16_t *startVal);

        /// @brief This function sets the start of measured interval in 
        ///  millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 250
        /// @return Error code (0 no error)
        int32_t setDistanceStart(uint16_t start);

        /// @brief This function returns the end of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 3000
        /// @return Error code (0 no error)
        int32_t getDistanceEnd(uint16_t *end);

        /// @brief This function sets the end of measured interval 
        ///  in millimeters. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 3000
        /// @return Error code (0 no error)
        int32_t setDistanceEnd(uint16_t end);

        /// @brief This function returns the used to limit step length.
        ///  If set to 0 (default), the step length is calculated 
        ///  based on profile. 
        ///  Default Value: 0
        /// @return Error code (0 no error)
        int32_t getDistanceMaxStepLength(uint16_t *length);

        /// @brief This function sets the used to limit step length.
        ///  If set to 0 (default), the step length is calculated 
        ///  based on profile. 
        ///  Default Value: 0
        /// @return Error code (0 no error)
        int32_t setDistanceMaxStepLength(uint16_t length);

        /// @brief This function reads if the close range leakage 
        ///  cancellation logic is enabled. 
        ///  Default Value: true 
        /// @return Error code (0 no error)
        int32_t getDistanceCloseRangeLeakageCancellation(bool *range);

        /// @brief This function sets the close range leakage 
        ///  cancellation logic. 
        ///  Default Value: true 
        /// @return Error code (0 no error)
        int32_t setDistanceCloseRangeLeakageCancellation(bool range);

        /// @brief This function returns the high signal quality in a 
        ///  better SNR (because of higher HWAAS) and higher power consumption.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 15000
        /// @return Error code (0 no error)
        int32_t getDistanceSignalQuality(uint16_t *signal);

        /// @brief This function sets the high signal quality in a 
        ///  better SNR (because of higher HWAAS) and higher power consumption.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 15000
        /// @return Error code (0 no error)
        int32_t setDistanceSignalQuality(uint16_t signal);

        /// @brief This function returns the max profile of the device. 
        ///  Default value = PROFILE5
        /// @return Error code (0 no error) 
        int32_t getDistanceMaxProfile(sfe_xm125_distance_profile_t *profile);

        /// @brief This function sets the max profile of the device 
        ///  Default value = PROFILE5
        /// @return Error code (0 no error)
        int32_t setDistanceMaxProfile(sfe_xm125_distance_profile_t profile);

        /// @brief This function returns the threshold method 
        /// @return Error code (0 no error)
        int32_t getDistanceThresholdMethod(sfe_xm125_threshold_method_t *method);

        /// @brief This function sets the threshold method
        /// @param method Threshold method (enum)
        /// @return Error code (0 no error)
        int32_t setDistanceThresholdMethod(sfe_xm125_threshold_method_t method);

        /// @brief This function returns the peak sorting method
        /// @param peak Peak sorting method 
        /// @return Error code (0 no error)
        int32_t getDistancePeakSorting(sfe_xm125_distance_peak_sorting_t *peak);

        /// @brief This function sets the peak sorting method 
        /// @param peak Peak sorting method 
        /// @return Error code (0 no error) 
        int32_t setDistancePeakSorting(sfe_xm125_distance_peak_sorting_t peak);

        /// @brief This function returns the number frames to use for recorded threshold. 
        ///  Default Value: 100
        /// @param thresh Number of frames 
        /// @return Error code (0 no error) 
        int32_t getDistanceNumFramesRecordedThreshold(uint16_t *thresh);

        /// @brief This function sets the number frames to use for recorded threshold. 
        ///  Default Value: 100
        /// @param thresh Number of frames 
        /// @return Error code (0 no error) 
        int32_t setDistanceNumFramesRecordedThreshold(uint16_t thresh);

        /// @brief This function returns the fixed amplitude threshold value. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 100000
        /// @param thresh Fixed amplitude threshold value
        /// @return Error code (0 no error) 
        int32_t getDistanceFixedAmpThreshold(uint16_t *thresh);

        /// @brief This function sets the fixed amplitude threshold value. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 100000
        /// @param thresh Fixed amplitude threshold value
        /// @return Error code (0 no error) 
        int32_t setDistanceFixedAmpThreshold(uint16_t thresh);

        /// @brief This function returns the threshold sensitivity 
        ///  (0 <= sensitivity <= 1000)
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 500
        /// @param thresh* Threshold sensitivity  
        /// @return Error code (0 no error) 
        int32_t getDistanceThresholdSensitivity(uint16_t *thresh);

        /// @brief This function sets the threshold sensitivity 
        ///  (0 <= sensitivity <= 1000)
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 500
        /// @param thresh Threshold sensitivity  
        /// @return Error code (0 no error) 
        int32_t setDistanceThresholdSensitivity(uint16_t thresh);

        /// @brief This function returns the reflector shape
        ///   Default Value: GENERIC
        /// @param shape Generic or planar reflection 
        /// @return Error code (0 no error) 
        int32_t getDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t *shape);

        /// @brief This function sets the reflector shape
        ///   Default Value: GENERIC
        /// @param shape Generic or planar reflection 
        /// @return Error code (0 no error) 
        int32_t setDistanceReflectorShape(sfe_xm125_distance_reflector_shape_t shape);

        /// @brief This function returns the fixed strength threshold value. 
        ///  Default Value: 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @param thresh fixed threshold strength value 
        /// @return Error code (0 no error) 
        int32_t getDistanceFixedStrengthThresholdValue(int16_t *thresh);

        /// @brief This function sets the fixed strength threshold value. 
        ///  Default Value: 0
        ///  Note: This value is a factor 1000 larger than the RSS value
        /// @param thresh fixed threshold strength value 
        /// @return Error code (0 no error) 
        int32_t setDistanceFixedStrengthThresholdValue(int16_t thresh);

        /// @brief This function returns the measure on wakeup status.
        ///   Default Value: false
        /// @param measure Measure on wakeup occurance 
        /// @return Error code (0 no error) 
        int32_t getDistanceMeasureOneWakeup(bool *measure);

        /// @brief This function sets the measure on wakeup status.
        ///   Default Value: false
        /// @param measure Measure on wakeup occurance 
        /// @return Error code (0 no error) 
        int32_t setDistanceMeasureOneWakeup(bool measure);

        /// @brief This function sets the specific execute command as defined in the 
        ///   datasheet on Page 25, Section 6.2.40 Command.
        /// @param apply Enable configuration and calibrate
        /// @return Error code (0 no error) 
        int32_t setCommand(sfe_xm125_distance_command_t *command);

        // --------------------- I2C Presence Detector Functions ---------------------


        /// @brief This function returns the RSS version number     
        /// @param version Version number
        /// @param patch Patch version number 
        /// @return Error code (0 no error)   
        int32_t getPresenceDetectorVersion(uint8_t *major, uint8_t *minor, uint8_t *patch);;

        /// @brief This function returns the protocol state erorr 
        /// @param state Error code for device 
        ///   0 = Protocol state error 
        ///   1 = Packet length error 
        ///   2 = Address error
        ///   3 = Write failed
        ///   4 = Write to read only 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorError(uint32_t *error);

        /// @brief This function returns the measure counter; the number of 
        ///  measurements performed since restart.
        /// @param counter number of measurements
        /// @return Error code (0 no error)  
        int32_t getPresenceMeasureCounter(uint32_t *counter);

        /// @brief This function returns the detector status flags. See page 17, section 
        ///   6.2.4 Detector Status 
        /// @param status Status detector flag 
        /// @return Error code (0 no error)  
        int32_t getPresenceDetectorStatus(uint32_t *status);

        /// @brief This function returns the number of detected distance. 
        /// @param detected Number of detected distance
        /// @return Error code (0 no error)  
        int32_t getPresenceDistanceResult(uint32_t *detected);

        /// @brief This function returns the indication that there might be an object near 
        ///  the start point of measure range
        /// @param nearEdge Object near start point
        /// @return Error code (0 no error) 
        int32_t getPresenceNearStartEdge(uint32_t *nearEdge);

        /// @brief This function the indication of sensor calibration required. The sensor
        ///  calibration needs to be redone. 
        /// @param calibrate Indication of sensor calibration 
        /// @return Error code (0 no error) 
        int32_t getPresenceCalibrationNeeded(uint32_t *calibrate);

        /// @brief This function returns the temperature in sensor during measurement (in
        ///   degree Celsius). Note that it has poor absolute accuracy and should only 
        ///   be used for relavtive temperature measurements. 
        /// @param temp Relative temperature
        /// @return Error code (0 no error) 
        int32_t getPresenceTemperature(uint32_t *temp);

        /// @brief This function returns the distance, in millimeters, for the detected
        ///  presence. 
        /// @param distance distance in mm
        /// @return Error code (0 no error) 
        int32_t getPresenceDistance(uint32_t *distance);

        /// @brief This function returns the measure of amount of fast motion detected. 
        /// @param intra Measure of fast motion 
        /// @return Error code (0 no error) 
        int32_t getPresenceIntraPresenceScore(uint32_t *intra);

        /// @brief This function returns the measure of amount of slow motion detected. 
        /// @param inter Measure of slow motion 
        /// @return Error code (0 no error) 
        int32_t getPresenceInterPresenceScore(uint32_t *inter);

        /// @brief This function returns the number of sweeps that will be 
        ///   captured in each frame (measurement).
        ///   Default Value: 16 seconds
        /// @param sweeps Number of sweeps
        /// @return Error code (0 no error) 
        int32_t getPresenceSweepsPerFrame(uint32_t *sweeps);

        /// @brief This function sets the number of sweeps that will be 
        ///   captured in each frame (measurement).
        ///   Default Value: 16 seconds
        /// @param sweeps Number of sweeps
        /// @return Error code (0 no error) 
        int32_t setPresenceSweepsPerFrame(uint32_t sweeps);

        /// @brief Returns the number of seconds the inter-frame presence score needs to decrease before
        ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
        ///  means no timeout.
        ///   Default Value: 3 seconds
        /// @param time Number of seconds 
        /// @return Error code (0 no error) 
        int32_t getPresenceInterFramePresenceTimeout(uint32_t *time);

        /// @brief Sets the number of seconds the inter-frame presence score needs to decrease before
        ///  exponential scaling starts for faster decline. Should be between 0 and 30 where 0
        ///  means no timeout.
        ///   Default Value: 3 seconds
        /// @param time Number of seconds 
        /// @return Error code (0 no error) 
        int32_t setPresenceInterFramePresenceTimeout(uint32_t time);

        /// @brief This function returns the enable to increase detection of slow 
        ///  motions by utilizing the phase information in the Sparse IQ data
        /// @param en Enables increased detection 
        ///   Default Value: false
        /// @return Error code (0 no error) 
        int32_t getPresenceInterPhaseBoostEnabled(bool *en);

        /// @brief This function sets the enable to increase detection of slow 
        ///  motions by utilizing the phase information in the Sparse IQ data
        /// @param en Enables increased detection 
        ///   Default Value: false
        /// @return Error code (0 no error) 
        int32_t setPresenceInterPhaseBoostEnabled(bool en);

        /// @brief This function returns the bit to enable to detect faster 
        ///  movements inside frames. 
        ///   Default Value: true
        /// @param en Enables faster detection 
        /// @return Error code (0 no error)
        int32_t getPresenceIntraDetectionEnabled(bool *en);

        /// @brief This function returns the bit to enable to detect slower
        ///  movements between frames. 
        ///   Default Value: true
        /// @param en Enables faster detection 
        /// @return Error code (0 no error)
        int32_t setPresenceInterDetectionEnabled(bool en);

        /// @brief This function returns the presence detector frame rate. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 12000 mHz
        /// @param rate Frame rate, in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceFrameRate(uint32_t *rate);

        /// @brief This function sets the presence detector frame rate. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 12000 mHz
        /// @param rate Frame rate, in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceFrameRate(uint32_t rate);

        /// @brief This function returns the threshold for detecting faster 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1300
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t getPresenceIntraDetectionThreshold(uint32_t *thresh);

        /// @brief This function sets the threshold for detecting faster 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1300
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t setPresenceIntraDetectionThreshold(uint32_t thresh);

        /// @brief This function returns the threshold for detecting slower 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1000
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t getPresenceInterDetectionThreshold(uint32_t *thresh);

        /// @brief This function sets the threshold for detecting slower 
        ///  movements inside frames. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 1000
        /// @param thresh threshold for faster movements 
        /// @return Error code (0 no error)
        int32_t setPresenceInterDetectionThreshold(uint32_t thresh);

        /// @brief This function returns the time constant of the low pass 
        ///  filter for the inter-frame deviation between fast and slow.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 500 ms
        /// @param time time in ms
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameDeviationTime(uint32_t *time);

        /// @brief This function sets the time constant of the low pass 
        ///  filter for the inter-frame deviation between fast and slow.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 500 ms
        /// @param time time in ms
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameDeviationTime(uint32_t time);

        /// @brief This function returns the cutoff frequency of the low pass 
        ///  filter for the fast filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 6000 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameFastCutoff(uint32_t *cut);

        /// @brief This function sets the cutoff frequency of the low pass 
        ///  filter for the fast filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 6000 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameFastCutoff(uint32_t cut);

        /// @brief This function returns the cutoff frequency of the low pass 
        ///  filter for the slow filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 200 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t getPresenceInterFrameSlowCutoff(uint32_t *cut);

        /// @brief This function sets the cutoff frequency of the low pass 
        ///  filter for the slow filtered absolute sweep mean.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 200 mHz
        /// @param cut cutoff frequency in mHz
        /// @return Error code (0 no error)
        int32_t setPresenceInterFrameSlowCutoff(uint32_t cut);

        /// @brief This functions returns time constant for the depthwise 
        ///  filtering in the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 150ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceIntraFrameTimeConst(uint32_t *time);

        /// @brief This functions sets time constant for the depthwise 
        ///  filtering in the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 150ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceIntraFrameTimeConst(uint32_t time);

        /// @brief This function returns time constant for the output in 
        ///  the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 300ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceIntraOutputTimeConst(uint32_t *time);

        /// @brief This function sets the time constant for the output in 
        ///  the intra-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 300ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceIntraOutputTimeConst(uint32_t time);

        /// @brief This function returns time constant for the output in 
        ///  the inter-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2000ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t getPresenceInterOutputTimeConst(uint32_t *time);

        /// @brief This function sets the time constant for the output in 
        ///  the inter-frame part
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2000ms
        /// @param time time const in ms
        /// @return Error code (0 no error)
        int32_t setPresenceInterOutputTimeConst(uint32_t time);

        /// @brief This function returns if enable/disable for the automatic  
        ///  selection of profile based on start point of measurement.
        ///   Default Value: true
        /// @param en enable/disable selection
        /// @return Error code (0 no error)
        int32_t getPresenceAutoProfileEn(bool *en);

        /// @brief This function enables/disables the automatic selection 
        ///  of profile based on start point of measurement.
        ///   Default Value: true
        /// @param en enable/disable selection
        /// @return Error code (0 no error)
        int32_t setPresenceAutoProfileEn(bool en);

        /// @brief This function returns the Enable/Disable 
        ///  automatic selection of step length based on the profile.
        /// @param en Enable/disable selection
        /// @return Error code (0 no error)
        int32_t getPresenceAutoStepLengthEn(bool *en);

        /// @brief This function Enables/Disables automatic selection
        ///  of step length based on the profile.
        /// @param en Enable/disable selection
        /// @return Error code (0 no error)
        int32_t setPresenceAutoStepLengthEn(bool en);

        /// @brief This function returns the profile in use. The profile will only 
        ///  be used if profile auto selection was disabled. 
        /// @param prof Profile to use (enum)
        /// @return Error code (0 no error)
        int32_t getPresenceManualProfile(sfe_xm125_presence_manual_profile_t *prof);

        /// @brief This function sets the profile to use. The profile will only 
        ///  be used if profile auto selection was disabled. 
        /// @param prof Profile to use (enum)
        /// @return Error code (0 no error)
        int32_t setPresenceManualProfile(sfe_xm125_presence_manual_profile_t prof);

        /// @brief THis function returns the number of steps between each data point. 
        ///  NOTE: The manual step length will only be used if step length auto selection 
        ///  was disabled
        ///   Default Value: 72
        /// @param length number of steps
        /// @return Error code (0 no error)
        int32_t getPresenceManualStepLength(uint32_t *length);

        /// @brief THis function returns the number of steps between each data point. 
        ///  NOTE: The manual step length will only be used if step length auto selection 
        ///  was disabled
        ///   Default Value: 72
        /// @param length number of steps 
        /// @return Error code (0 no error)
        int32_t setPresenceManualStepLength(uint32_t length);

        /// @brief This function returns the start of measured interval in mm. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 300mm
        /// @param start measured start interval in mm
        /// @return Error code (0 no error) 
        int32_t getPresenceStart(uint32_t *start);

        /// @brief This function sets the start of measured interval in mm. 
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///  Default Value: 300mm
        /// @param start measured start interval in mm
        /// @return Error code (0 no error) 
        int32_t setPresenceStart(uint32_t start);

        /// @brief This function returns the end point of measurement 
        ///  interval in millimeters.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2500
        /// @param end End point of measurement in mm 
        /// @return Error code (0 no error) 
        int32_t getPresenceEnd(uint32_t *end);

        /// @brief This function returns the end point of measurement 
        ///  interval in millimeters.
        ///  Note: This value is a factor 1000 larger than the RSS value
        ///   Default Value: 2500
        /// @param end End point of measurement in mm 
        /// @return Error code (0 no error) 
        int32_t setPresenceEnd(uint32_t end);

        /// @brief This function returns if the device is enabled/disabled 
        ///  for the reset of the presence filters during start/restart.
        ///   Default Value: true
        /// @param reset enabled/disabled 
        /// @return Error code (0 no error) 
        int32_t getPresenceResetFilters(bool *reset);

        /// @brief This function sets if the device is enabled/disabled 
        ///  for the reset of the presence filters during start/restart .
        ///   Default Value: true
        /// @param reset enabled/disabled 
        /// @return Error code (0 no error) 
        int32_t setPresenceResetFilters(bool reset);

        /// @brief This function returns the hardware accelerated average
        ///  sampled (HWAAS).
        ///   Default Value: 32
        /// @param avg average samples 
        /// @return Error code (0 no error) 
        int32_t getPresenceHWAAS(uint32_t *avg);

        /// @brief This function sets the hardware accelerated average
        ///  sampled (HWAAS).
        ///   Default Value: 32
        /// @param avg average samples 
        /// @return Error code (0 no error) 
        int32_t setPresenceHWAAS(uint32_t avg);

        /// @brief This function returns the output presence detection on 
        ///  generic GPIO.
        /// @param detected Output presence detected 
        /// @return Error code (0 no error) 
        int32_t getPresenceDetectionOnGPIO(bool *detected);

        /// @brief This function returns the output presence detection on 
        ///  generic GPIO. 
        /// @param detected Output presence detected 
        /// @return Error code (0 no error) 
        int32_t setPresenceDetectionOnGPIO(bool detected); 

        /// @brief This function sets the execute command
        /// @param cmd command to send to device 
        /// @return Error code (0 no error) 
        int32_t setPresenceCommand(sfe_xm125_presence_command_t cmd);


// Bad code - need to access registers instead

        // // --------------------- I2C Application System functions ---------------------
    
        // /// @brief Disable interrupts
        // /// @return Error code (0 no error)
        // int32_t disableInterrupts(); // disable_interrupts

        // /// @brief Enable interrupts
        // /// @return Error code (0 no error) 
        // int32_t enableInterrupts(); // enable_interrupts

        // /// @brief Prepare power down state
        // /// @return Error code (0 no error)
        // int32_t preparePowerDown(); // prepare_power_down

        // /// @brief Resume power down state
        // /// @return Error code (0 no error)
        // int32_t resumePowerDown(); // resume_power_down

        // /// @brief Get RTC ticks based on current RTC time
        // /// @return Error code (0 no error)
        // int32_t getRTCTick(); // get_rtc_tick

        // /// @brief Convert RTC ticks to RTC time 
        // /// @param tick rtc ticks in ms
        // /// @param time RTC time
        // /// @return Error code (0 no error)
        // int32_t rtcTickToTime(uint32_t tick, RTC_TimeTypeDef *time); // rtc_tick_to_time

        // /// @brief Function for setting the next wakeup time from the 
        // ///  RTC interrupt
        // /// @return Error code (0 no error)
        // int32_t setRTCWakeupTime();

        // /// @brief Wait for I2C interface to be idle
        // /// @return Error code (0 no error)
        // int32_t waitForI2CIdle(); // wait_for_i2c_idle

        // /// @brief Helper function to prepare transmit of register data
        // /// @return Error code (0 no error)
        // int32_t prepareRegisterData(); // prepare_register_data

        // /// @brief Initializes the STM32 I2C communication 
        // /// @return Error code (0 no error)
        // int32_t initI2CApplicationSystem(); // i2c_application_system_init

        // /// @brief Wait for I2C to idle, then reset the system
        // /// @return Error code (0 no error)
        // int32_t resetI2CApplicationSystem(); // i2c_application_system_reset

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t i2cApplicationSystemWaitForInterrupt(); // i2c_application_system_wait_for_interrupt

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t i2cApplicationSystemTestWakeupPin(); // i2c_application_system_test_wakeup_pin
        
        // /// @brief Function to enable the Reset pin state
        // /// @param en 0 disable, 1 enable
        // /// @return Error code (0 no error)
        // int32_t setI2CApplicationSystemReadyPin(bool en); // i2c_application_system_set_ready_pin
        
        // /// @brief Function to set GPIO0 to pull or no pull
        // /// @param en 0 disable, 1 enable
        // /// @return Error code (0 no error)
        // int32_t configureI2CApplicationGPIO0Pin(bool en); // i2c_application_system_setup_generic_gpio_pin
        
        // /// @brief Function to enable the GPIO0 state 
        // /// @param en 0 disable, 1 enable
        // /// @return Error code (0 no error)
        // int32_t setI2CApplicationGPIO0Pin(bool en); // i2c_application_system_set_generic_gpio_pin
 
        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t enterI2CApplicationLowerPowerState(); // i2c_application_enter_low_power_state

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t setI2CApplicationPeriodicWakeup(); // i2c_application_set_periodic_wakeup

        // /// @brief Returns an error code for the I2C Handler
        // /// @return Error code (0 no error)
        // int32_t errorI2CCallback(); // HAL_I2C_ErrorCallback

        // // --------------------- I2C Disance Detector Functions ---------------------
        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t getI2CDistanceDetectorConfig(acc_detector_distance_config_t *config);

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t distanceDetectorCommand(uint32_t *cmd);

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t getDistanceDetectorStatus(uint32_t *status);

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t getDistanceDetectorResult(uint32_t *result);

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t getDistanceDetectorCounter(uint32_t *count);

        // /// @brief 
        // /// @return Error code (0 no error)
        // int32_t getDistanceDetectorPeakDistance(float *distance);

        // /// @brief 
        // /// @return Error code (0 no error) 
        // int32_t getDistanceDetectorPeakStrength(float *peak, uint16_t id);

        // /// @brief 
        // /// @return Error code (0 no error) 
        // int32_t setDistanceDetectorMeasureOnWakeup(bool *en);

        // /// @brief 
        // /// @return Error code (0 no error) 
        // int32_t getDistanceDetectorMeasureOnWakeup(bool *meas);

        // // --------------------- I2C Presence Detector Functions ---------------------
        // /// @brief 
        // /// @return Error code (0 no error)  
        // int32_t presenceDetectorCommand(uint32_t cmd);

        // /// @brief 
        // /// @return Error code (0 no error)  
        // int32_t getpresenceDetectorConfig(acc_detector_presence_config_t *config);

        // /// @brief 
        // /// @return Error code (0 no error) 
        // int32_t getPresenceDetectorStatus(uint32_t *status);

        // /// @brief 
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorResult(uint32_t *result);

        // /// @brief the distance, in millimeters, for the detected presence
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorDistance(float *distance);

        // /// @brief Returns the measure of the amount of fast motion detected
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorIntraPresenceScore(float *intraScore);

        // /// @brief Returns the measure of the amount of slow motion detected
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorInterPresenceScore(float *interScore);

        // /// @brief Get the measure counter, the number of measurements performed
        // ///  since restart. 
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorCounter(uint32_t *counter);

        // /// @brief 
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorGPIODetection(bool *en);

        // /// @brief 
        // /// @return Error code (0 no error)  
        // int32_t getPresenceDetectorGPIODetected(bool detected);

};
