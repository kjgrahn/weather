.PHONY: foo
foo: post
	curl -LO --data-binary @post 'http://api.trafikinfo.trafikverket.se/v1.3/data.xml'
