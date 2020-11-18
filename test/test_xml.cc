#include <sample.h>

#include <orchis.h>

namespace {
    std::string observation(const char* name)
    {
	std::string s;
	s += "    <WeatherObservation>"
	    "      <Sample>2020-11-16T22:40:00</Sample>"
	    "      <Surface>"
	    "        <Temperature>"
	    "          <Origin>measured</Origin>"
	    "          <SensorNames>PT100</SensorNames>"
	    "          <Value>7.8</Value>"
	    "        </Temperature>"
	    "      </Surface>"
	    "      <Air>"
	    "        <Temperature>"
	    "          <Origin>measured</Origin>"
	    "          <SensorNames>LAMBRECHT_1</SensorNames>"
	    "          <Value>8.6</Value>"
	    "        </Temperature>"
	    "        <Dewpoint>"
	    "          <Origin>calculated</Origin>"
	    "          <SensorNames>LAMBRECHT_1</SensorNames>"
	    "          <Value>8</Value>"
	    "        </Dewpoint>"
	    "        <RelativeHumidity>"
	    "          <Origin>measured</Origin>"
	    "          <SensorNames>LAMBRECHT_1</SensorNames>"
	    "          <Value>95.5</Value>"
	    "        </RelativeHumidity>"
	    "      </Air>"
	    "      <Wind>"
	    "        <Height>6</Height>"
	    "        <Speed>"
	    "          <Origin>measured</Origin>"
	    "          <SensorNames>WAA15</SensorNames>"
	    "          <Value>5.3</Value>"
	    "        </Speed>"
	    "        <Direction>"
	    "          <Origin>measured</Origin>"
	    "          <SensorNames>WAC15</SensorNames>"
	    "          <Value>225</Value>"
	    "        </Direction>"
	    "      </Wind>"
	    "      <Aggregated10minutes>"
	    "        <Precipitation>"
	    "          <Rain>false</Rain>"
	    "          <Snow>true</Snow>"
	    "          <RainSum>"
	    "            <Origin>calculated</Origin>"
	    "            <SensorNames>OPTICEYE</SensorNames>"
	    "            <Value>0</Value>"
	    "          </RainSum>"
	    "          <SnowSum>"
	    "            <Solid>"
	    "              <Origin>calculated</Origin>"
	    "              <SensorNames>OPTICEYE</SensorNames>"
	    "              <Value>0</Value>"
	    "            </Solid>"
	    "          </SnowSum>"
	    "          <TotalWaterEquivalent>"
	    "            <Value>1.3</Value>"
	    "          </TotalWaterEquivalent>"
	    "        </Precipitation>"
	    "      </Aggregated10minutes>"
	    "      <Aggregated30minutes>"
	    "        <Wind>"
	    "          <SpeedMax>"
	    "            <Value>8.8</Value>"
	    "          </SpeedMax>"
	    "        </Wind>"
	    "        <Precipitation>"
	    "          <Rain>false</Rain>"
	    "          <Snow>false</Snow>"
	    "          <RainSum>"
	    "            <Origin>calculated</Origin>"
	    "            <SensorNames>OPTICEYE</SensorNames>"
	    "            <Value>0</Value>"
	    "          </RainSum>"
	    "          <SnowSum>"
	    "            <Solid>"
	    "              <Origin>calculated</Origin>"
	    "              <SensorNames>OPTICEYE</SensorNames>"
	    "              <Value>0</Value>"
	    "            </Solid>"
	    "          </SnowSum>"
	    "          <TotalWaterEquivalent>"
	    "            <Value>0</Value>"
	    "          </TotalWaterEquivalent>"
	    "        </Precipitation>"
	    "      </Aggregated30minutes>"
	    "      <Id>7099641</Id>"
	    "      <Measurepoint>"
	    "        <Id>";
	s += name;
	s += "</Id>"
	    "        <Name>Marka</Name>"
	    "        <Geometry>"
	    "          <SWEREF99TM>POINT (409003.997 6447971.304)</SWEREF99TM>"
	    "          <WGS84>POINT (13.4534 58.16363)</WGS84>"
	    "        </Geometry>"
	    "      </Measurepoint>"
	    "      <ModifiedTime>2020-11-16T21:41:33.956Z</ModifiedTime>"
	    "    </WeatherObservation>";
	return s;
    }
}

namespace xml {

    void plain(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>" + observation("1617") +
	    "  </RESULT>"
	    "</RESPONSE>";

	auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 1);

	const auto& series = samples["1617"];

	{
	    orchis::assert_eq(series.size(), 1);
	    Sample s = series[0];
	    auto assert_val = [&s] (const char* name, const char* val) {
				  orchis::assert_eq(s.data[name], val);
			      };

	    orchis::assert_eq(s.time, "2020-11-16T22:40:00");
	    assert_val("temperature.road", "7.8");
	    assert_val("temperature.air",  "8.6");
	    assert_val("wind.direction",   "225");
	    assert_val("wind.force",       "5.3");
	    assert_val("wind.force.max",   "8.8");
	    assert_val("humidity",         "95.5");

	    assert_val("rain.amount",       "7.8");
	    assert_val("rain.type",         "snow");
	}
    }

    void three_stations(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>" + observation("foo") + observation("bar") + observation("baz") +
	    "  </RESULT>"
	    "</RESPONSE>";

	const auto samples = parse(doc);
	orchis::assert_eq(samples.count("foo"), 1);
	orchis::assert_eq(samples.count("bar"), 1);
	orchis::assert_eq(samples.count("baz"), 1);
	orchis::assert_eq(samples.size(), 3);

	for(const auto& val : samples) {
	    const auto& series = val.second;

	    orchis::assert_eq(series.size(), 1);
	    Sample s = series[0];
	    auto assert_val = [&s] (const char* name, const char* val) {
				  orchis::assert_eq(s.data[name], val);
			      };

	    orchis::assert_eq(s.time, "2020-11-16T22:40:00");
	    assert_val("temperature.road", "7.8");
	    assert_val("temperature.air",  "8.6");
	    assert_val("wind.direction",   "225");
	    assert_val("wind.force",       "5.3");
	    assert_val("wind.force.max",   "8.8");
	    assert_val("humidity",         "95.5");

	    assert_val("rain.amount",       "7.8");
	    assert_val("rain.type",         "snow");
	}
    }

    void empty(orchis::TC)
    {
	const std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT/>"
	    "</RESPONSE>";

	const auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 0);
    }

    void truncated(orchis::TC)
    {
	std::string doc = "<?xml version='1.0'?>"
	    "<RESPONSE>"
	    "  <RESULT>" + observation("foo") + observation("bar") + observation("baz") +
	    "  </RESULT>"
	    "</RESPONSE>";

	doc.resize(doc.size() - 10);

	const auto samples = parse(doc);
	orchis::assert_eq(samples.size(), 0);
    }
}
