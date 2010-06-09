make
./parse-bacs nintendochan.cs > /www/tracescript/blog/nintendochan.cs.new.txt
./parse-bacs banner.cs > /www/tracescript/blog/banner.cs.new.txt
./parse-bacs icon.cs > /www/tracescript/blog/icon.cs.new.txt
./parse-bacs moo.cs > /www/tracescript/blog/moo.cs.new.txt
./parse-bacs wiimusic_onrecv.kwb > /www/tracescript/blog/wiimusic.cs.new.txt
./addbom /www/tracescript/blog/nintendochan.cs.new.txt /www/tracescript/blog/bom.nintendochan.cs.new.txt
./addbom /www/tracescript/blog/banner.cs.new.txt /www/tracescript/blog/bom.banner.cs.new.txt
./addbom /www/tracescript/blog/icon.cs.new.txt /www/tracescript/blog/bom.icon.cs.new.txt
./addbom /www/tracescript/blog/moo.cs.new.txt /www/tracescript/blog/bom.moo.cs.new.txt
./addbom /www/tracescript/blog/wiimusic.cs.new.txt /www/tracescript/blog/bom.wiimusic.cs.new.txt

