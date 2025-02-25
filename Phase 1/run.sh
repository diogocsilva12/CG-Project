#!/bin/bash

while true; do
    clear
    echo "====== 3D Figure Generator ======"
    echo "1. Clean and Build"
    echo "2. Generate Figure"
    echo "3. View Generated Figure"
    echo "4. Run Test Files"
    echo "0. Exit"
    echo "==========================================="
    
    read -p "Choose an option: " option

    case $option in
        1)
            echo "Cleaning and building..."
            if [ -d "build" ]; then
                cd build
                rm -rf *
            else
                mkdir build
                cd build
            fi
            cmake ..
            make
            cd ..
            ;;
            
        2)
            echo "=== Generate Figure ==="
            echo "1. Plane"
            echo "2. Box"
            echo "3. Sphere"
            echo "4. Cone"
            echo "0. Back"
            
            read -p "Select figure type: " figure
            
            case $figure in
            1)
                echo "=== Plane Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=1, slices=3"
                read -p "Enter unit size (default=1): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-1}
                slices=${slices:-3}
                cd generator
                ./generator plane $unit $slices plane.3d
                cd ..
                ;;
            2)
                echo "=== Box Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=2, slices=3"
                read -p "Enter unit size (default=2): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-2}
                slices=${slices:-3}
                cd generator
                ./generator box $unit $slices box.3d
                cd ..
                ;;
            3)
                echo "=== Sphere Parameters ==="
                echo "Example: radius=2, slices=10, stacks=10"
                echo "Default: radius=1, slices=10, stacks=10"
                read -p "Enter radius (default=1): " radius
                read -p "Enter slices (default=10): " slices
                read -p "Enter stacks (default=10): " stacks
                radius=${radius:-1}
                slices=${slices:-10}
                stacks=${stacks:-10}
                cd generator
                ./generator sphere $radius $slices $stacks sphere.3d
                cd ..
                ;;
            4)
                echo "=== Cone Parameters ==="
                echo "Example: radius=2, height=4, slices=10, stacks=10"
                echo "Default: radius=1, height=2, slices=4, stacks=3"
                read -p "Enter radius (default=1): " radius
                read -p "Enter height (default=2): " height
                read -p "Enter slices (default=4): " slices
                read -p "Enter stacks (default=3): " stacks
                radius=${radius:-1}
                height=${height:-2}
                slices=${slices:-4}
                stacks=${stacks:-3}
                cd generator
                ./generator cone $radius $height $slices $stacks cone.3d
                cd ..
                ;;
                        0)
                            continue
                            ;;
                    esac
                    ;;
            
       
        3)
            echo "=== View Generated Figures ==="
            echo "Available figures:"
            
            if [ -d "tests" ]; then
                files=(tests/*.3d)
                if [ ${#files[@]} -eq 0 ] || [ ! -e "${files[0]}" ]; then
                    echo "No generated figures found."
                else
                    for file in "${files[@]}"; do
                        filename=$(basename "$file")
                        echo "- ${filename%.*}"
                    done
                    

                    read -p "0 to go back: " figure_name
                    
                    case $figure_name in
                        0)
                            continue
                            ;;
                    esac
                fi
            else
                echo "No figures directory found!"
                read -p "Press Enter to continue..."
            fi
            ;;

        4)
    echo "=== Available Test Files ==="
    if [ -d "engine/configs" ]; then
        # Only show files that start with "test"
        files=(engine/configs/test*.xml)
        if [ ${#files[@]} -eq 0 ] || [ ! -e "${files[0]}" ]; then
            echo "No test files found."
        else
            echo "Test configurations:"
            # Create an array to store the filenames
            declare -a test_files
            counter=1
            
            # List files with numbers
            for file in "${files[@]}"; do
                filename=$(basename "$file" .xml)
                echo "$counter) ${filename}"
                test_files[$counter]=$filename
                ((counter++))
            done
            
            echo "0. Back to main menu"
            read -p "Select test number: " test_number
            
            case $test_number in
                0)
                    continue
                    ;;
                *)
                    if [ "$test_number" -ge 1 ] && [ "$test_number" -lt "$counter" ]; then
                        selected_test="${test_files[$test_number]}"
                        if [ -f "engine/configs/${selected_test}.xml" ]; then
                            cd build
                            ./engine "../engine/configs/${selected_test}.xml"
                            cd ..
                        fi
                    else
                        echo "Invalid test number!"
                        read -p "Press Enter to continue..."
                    fi
                    ;;
            esac
        fi
    else
        echo "No test configs directory found!"
        read -p "Press Enter to continue..."
    fi
    ;;


            
        0)
            echo "Exiting..."
            exit 0
            ;;
            
        *)
            echo "Invalid option"
            ;;
    esac
    
    read -p "Press Enter to continue..."
done