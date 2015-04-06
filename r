# echo "-------------------------- testing p4 stuff -------------------------"

# echo "testing views"
# ./p5exe < test4/view_in.txt > out
# diff out tmp/view_out.txt
# echo "testing status"
# ./p5exe < test4/status_in.txt > out
# diff out tmp/status_out.txt
# echo "testing show"
# ./p5exe < test4/show_in.txt > out
# diff out tmp/show_out.txt
# echo "testing errors"
# ./p5exe < test4/errors_in.txt > out
# diff out tmp/errors_out.txt
# echo "testing ship_dock"
# ./p5exe < test4/ship_dock_in > out
# diff out tmp/ship_dock_out
# echo "testing tanker load"
# ./p5exe < test4/tanker_load_in > out
# diff out tmp/tanker_load_out



echo "-------------------------- testing p5 stuff -------------------------"
echo "testing cruise"
./p5exe < test/cruise_in.txt > out
diff out test/cruise_out.txt
echo "testing status"
./p5exe < test/status_in.txt > out
diff out test/status_out.txt
echo "testing views"
./p5exe < test/views_in.txt > out
diff out test/views_out.txt