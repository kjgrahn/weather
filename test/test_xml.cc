#include <sample.h>

#include <orchis.h>

namespace xml {

    void plain(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>"
	    "    <WeatherStation>"
	    "      <Active>true</Active>"
	    "      <CountyNo>14</CountyNo>"
	    "      <Geometry>"
	    "        <SWEREF99TM>POINT (416544.83 6447184.97)</SWEREF99TM>"
	    "        <WGS84>POINT (13.58180046081543 58.158058166503906)</WGS84>"
	    "      </Geometry>"
	    "      <IconId>weatherStation</IconId>"
	    "      <Id>SE_STA_VVIS1617</Id>"
	    "      <Measurement>"
	    "        <MeasureTime>2018-09-27T00:30:00</MeasureTime>"
	    "        <Precipitation>"
	    "          <AmountName>Ingen nederb&#xF6;rd</AmountName>"
	    "          <Type>Ingen nederb&#xF6;rd</Type>"
	    "          <TypeIconId>precipitationNoPrecipitation</TypeIconId>"
	    "        </Precipitation>"
	    "        <Road>"
	    "          <Temp>9.3</Temp>"
	    "          <TempIconId>tempAirRoad</TempIconId>"
	    "        </Road>"
	    "        <Air>"
	    "          <Temp>10.9</Temp>"
	    "          <TempIconId>tempAirRoad</TempIconId>"
	    "          <RelativeHumidity>78.8</RelativeHumidity>"
	    "        </Air>"
	    "        <Wind>"
	    "          <Direction>270</Direction>"
	    "          <DirectionIconId>windW</DirectionIconId>"
	    "          <DirectionText>V&#xE4;st</DirectionText>"
	    "          <Force>7.4</Force>"
	    "          <ForceMax>12.6</ForceMax>"
	    "        </Wind>"
	    "      </Measurement>"
	    "      <MeasurementHistory>"
	    "        <MeasureTime>2018-09-27T00:20:00</MeasureTime>"
	    "        <Precipitation>"
	    "          <AmountName>Ingen nederb&#xF6;rd</AmountName>"
	    "          <Type>Ingen nederb&#xF6;rd</Type>"
	    "          <TypeIconId>precipitationNoPrecipitation</TypeIconId>"
	    "        </Precipitation>"
	    "        <Road>"
	    "          <Temp>9.3</Temp>"
	    "          <TempIconId>tempAirRoad</TempIconId>"
	    "        </Road>"
	    "        <Air>"
	    "          <Temp>11</Temp>"
	    "          <TempIconId>tempAirRoad</TempIconId>"
	    "          <RelativeHumidity>78.6</RelativeHumidity>"
	    "        </Air>"
	    "        <Wind>"
	    "          <Direction>270</Direction>"
	    "          <DirectionIconId>windW</DirectionIconId>"
	    "          <DirectionText>V&#xE4;st</DirectionText>"
	    "          <Force>7.7</Force>"
	    "          <ForceMax>13</ForceMax>"
	    "        </Wind>"
	    "      </MeasurementHistory>"
	    "      <ModifiedTime>2018-09-26T22:35:13.009Z</ModifiedTime>"
	    "      <Name>Falk&#xF6;ping</Name>"
	    "      <RoadNumberNumeric>47</RoadNumberNumeric>"
	    "    </WeatherStation>"
	    "  </RESULT>"
	    "</RESPONSE>";

	const auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 2);

	{
	    Sample s = samples[0];
	    auto assert_val = [&s] (const char* name, const char* val) {
				  orchis::assert_eq(s.data[name], val);
			      };

	    orchis::assert_eq(s.time, "2018-09-27T00:30:00");
	    assert_val("temperature.road", "9.3");
	    assert_val("temperature.air",  "10.9");
	    assert_val("wind.direction",   "270");
	    assert_val("wind.force",       "7.4");
	    assert_val("wind.force.max",   "12.6");
	    assert_val("humidity",         "78.8");
	}
    }

    void empty(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>"
	    "    <WeatherStation>"
	    "      <Active>true</Active>"
	    "      <CountyNo>14</CountyNo>"
	    "      <Geometry>"
	    "        <SWEREF99TM>POINT (416544.83 6447184.97)</SWEREF99TM>"
	    "        <WGS84>POINT (13.58180046081543 58.158058166503906)</WGS84>"
	    "      </Geometry>"
	    "      <IconId>weatherStation</IconId>"
	    "      <Id>SE_STA_VVIS1617</Id>"
	    "      <ModifiedTime>2018-09-26T22:35:13.009Z</ModifiedTime>"
	    "      <Name>Falk&#xF6;ping</Name>"
	    "      <RoadNumberNumeric>47</RoadNumberNumeric>"
	    "    </WeatherStation>"
	    "  </RESULT>"
	    "</RESPONSE>";

	const auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 0);
    }

    void truncated(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>"
	    "    <WeatherStation>"
	    "      <Active>true</Active>"
	    "      <CountyNo>14</CountyNo>"
	    "      <Geometry>"
	    "        <SWEREF99TM>POINT (416544.83 6447184.97)</SWEREF99TM>"
	    "        <WGS84>POINT (13.58180046081543 58.158058166503906)</WGS84>"
	    "      </Geometry>"
	    "      <IconId>weatherStation</IconId>"
	    "      <Id>SE_STA_VVIS1617</Id>"
	    "      <Measurement>";

	const auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 0);
    }
}
