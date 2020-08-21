        lxi  b,0003h  ; init timer delay value for timer, we set this quite high because fast feedback is important

	jmp disp_inc_end
display_out:
        lxi  h,0000h  ; init h:l for timer
        ; we display the contents of the output register by repeating this in a loop
disp_out_loop:
        ldax d
        ldax d
        ldax d
        ldax d
        dad  b
        jnc  disp_out_loop
        ret
disp_inc_end:

	in 0ffh                 ; read the sense switches
	sta input_var           ; store it into input_var
main_loop:
	lda input_var
	rlc                     ; rotate everything left
	sta input_var

	mov d,a
	call display_out

	; allow to update the pattern by setting a new one and then raising A15
	in 0ffh
	ani 080h
	cnz update_it
	jmp main_loop

rotate_it:
	rlc      ; rotate everything left
	ret

update_it:
	in 0ffh
	ani 07fh  ; lose the A15 bit
	rrc
	sta input_var
	ret

input_var:
	db 0