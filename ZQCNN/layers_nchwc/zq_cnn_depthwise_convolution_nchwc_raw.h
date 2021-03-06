void zq_cnn_depthwise_conv_no_padding_nchwc_general(
	const zq_base_type* in_tensor4D_data,
	int in_N,
	int in_H,
	int in_W,
	int in_C,
	int in_widthStep,
	int in_sliceStep,
	int in_imStep,
	const zq_base_type* filters_data,
	int filter_N, //must be 1
	int filter_H,
	int filter_W,
	int filter_C, // must be in_C
	int filter_widthStep,
	int filter_sliceStep,
	int filter_imStep,
	int stride_H,
	int stride_W,
	int dilate_H,
	int dilate_W,
	zq_base_type* out_tensor4D_data,
	int out_N,	// must be in_N
	int out_H,	// must be (in_H - filter_H)/stride_H + 1
	int out_W,	// must be (in_W - filter_W)/stride_W + 1
	int out_C,	// must be in_C
	int out_widthStep,
	int out_sliceStep,
	int out_imStep
#if WITH_BIAS
	, const zq_base_type* bias
#endif
#if WITH_PRELU
	, const zq_base_type* slope
#endif
)
{
	const zq_base_type* in_slice_ptr;
	const zq_base_type* in_row_ptr;
	const zq_base_type* in_pix_ptr;
	const zq_base_type* in_im_ptr;
	const zq_base_type* filter_slice_ptr;
	zq_base_type* out_slice_ptr;
	zq_base_type* out_row_ptr;
	zq_base_type* out_pix_ptr;
	zq_base_type* out_im_ptr;

	const zq_base_type* cur_in_row_ptr;
	const zq_base_type* cur_in_pix_ptr;
	const zq_base_type* cur_filter_row_ptr;
	const zq_base_type* cur_filter_pix_ptr;
	register zq_mm_type value_v;
#if WITH_BIAS
	register zq_mm_type bias_v;
#endif
#if WITH_PRELU
	register zq_mm_type slope_v;
	register zq_mm_type zero_v = zq_mm_setzero_ps();
#endif
	int stride_H_mul_in_widthStep = stride_H*in_widthStep;
	int stride_W_mul_in_pixStep = stride_W*zq_mm_align_size;
	int dilate_H_mul_in_widthStep = dilate_H*in_widthStep;
	int dilate_W_mul_in_pixStep = dilate_W*zq_mm_align_size;
	int out_n, out_h, out_w, out_c, kh, kw;

	for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
		out_n < out_N;
		out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
	{
		for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
			out_c < in_C;
			out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
		{
#if WITH_BIAS
			bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
			slope_v = zq_mm_load_ps(slope + out_c);
#endif
			for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
				out_h < out_H;
				out_h++, in_row_ptr += stride_H_mul_in_widthStep, out_row_ptr += out_widthStep)
			{
				for (out_w = 0, in_pix_ptr = in_row_ptr, out_pix_ptr = out_row_ptr;
					out_w < out_W;
					out_w++, in_pix_ptr += stride_W_mul_in_pixStep, out_pix_ptr += zq_mm_align_size)
				{
#if WITH_BIAS
					value_v = bias_v;
#else
					value_v = zq_mm_setzero_ps();
#endif
					for (kh = 0, cur_in_row_ptr = in_pix_ptr, cur_filter_row_ptr = filter_slice_ptr;
						kh < filter_H;
						kh++, cur_in_row_ptr += dilate_H_mul_in_widthStep, cur_filter_row_ptr += filter_widthStep)
					{
						for (kw = 0, cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
							kw < filter_W;
							kw++, cur_in_pix_ptr += dilate_W_mul_in_pixStep, cur_filter_pix_ptr += zq_mm_align_size)
						{
							value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
						}
					}
#if WITH_PRELU
					value_v = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v), zq_mm_max_ps(zero_v, value_v));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v);
				}
			}
		}
	}
}

void zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3(
	const zq_base_type* in_tensor4D_data,
	int in_N,
	int in_H,
	int in_W,
	int in_C,
	int in_widthStep,
	int in_sliceStep,
	int in_imStep,
	const zq_base_type* filters_data,
	int filter_N, //must be 1
	int filter_H,
	int filter_W,
	int filter_C, // must be in_C
	int filter_widthStep,
	int filter_sliceStep,
	int filter_imStep,
	int stride_H,
	int stride_W,
	int dilate_H,
	int dilate_W,
	zq_base_type* out_tensor4D_data,
	int out_N,	// must be in_N
	int out_H,	// must be (in_H - filter_H)/stride_H + 1
	int out_W,	// must be (in_W - filter_W)/stride_W + 1
	int out_C,	// must be in_C
	int out_widthStep,
	int out_sliceStep,
	int out_imStep
#if WITH_BIAS
	, const zq_base_type* bias
#endif
#if WITH_PRELU
	, const zq_base_type* slope
#endif
)
{
	const zq_base_type* in_slice_ptr;
	const zq_base_type* in_row_ptr;
	const zq_base_type* in_pix_ptr;
	const zq_base_type* in_im_ptr;
	const zq_base_type* filter_slice_ptr;
	zq_base_type* out_slice_ptr;
	zq_base_type* out_row_ptr;
	zq_base_type* out_pix_ptr;
	zq_base_type* out_im_ptr;

	const zq_base_type* cur_in_row_ptr;
	const zq_base_type* cur_in_pix_ptr;
	const zq_base_type* cur_filter_row_ptr;
	const zq_base_type* cur_filter_pix_ptr;
	register zq_mm_type value_v;
#if WITH_BIAS
	register zq_mm_type bias_v;
#endif
#if WITH_PRELU
	register zq_mm_type slope_v;
	register zq_mm_type zero_v = zq_mm_setzero_ps();
#endif
	int stride_H_mul_in_widthStep = stride_H*in_widthStep;
	int stride_W_mul_in_pixStep = stride_W*zq_mm_align_size;
	int dilate_H_mul_in_widthStep = dilate_H*in_widthStep;
	int dilate_W_mul_in_pixStep = dilate_W*zq_mm_align_size;
	int out_n, out_h, out_w, out_c;

	for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
		out_n < out_N;
		out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
	{
		for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
			out_c < in_C;
			out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
		{
#if WITH_BIAS
			bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
			slope_v = zq_mm_load_ps(slope + out_c);
#endif
			for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
				out_h < out_H;
				out_h++, in_row_ptr += stride_H_mul_in_widthStep, out_row_ptr += out_widthStep)
			{
				for (out_w = 0, in_pix_ptr = in_row_ptr, out_pix_ptr = out_row_ptr;
					out_w < out_W;
					out_w++, in_pix_ptr += stride_W_mul_in_pixStep, out_pix_ptr += zq_mm_align_size)
				{
					cur_in_row_ptr = in_pix_ptr; cur_filter_row_ptr = filter_slice_ptr;
					cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
#if WITH_BIAS
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), bias_v);
#else
					value_v = zq_mm_mul_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr));
#endif
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);

					cur_in_row_ptr += dilate_H_mul_in_widthStep; cur_filter_row_ptr += filter_widthStep;
					cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);

					cur_in_row_ptr += dilate_H_mul_in_widthStep; cur_filter_row_ptr += filter_widthStep;
					cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);

#if WITH_PRELU
					value_v = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v), zq_mm_max_ps(zero_v, value_v));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v);
				}
			}
		}
	}
}

void zq_cnn_depthwise_conv_no_padding_nchwc_kernel3x3_s1d1(
	const zq_base_type* in_tensor4D_data,
	int in_N,
	int in_H,
	int in_W,
	int in_C,
	int in_widthStep,
	int in_sliceStep,
	int in_imStep,
	const zq_base_type* filters_data,
	int filter_N, //must be 1
	int filter_H,
	int filter_W,
	int filter_C, // must be in_C
	int filter_widthStep,
	int filter_sliceStep,
	int filter_imStep,
	int stride_H,
	int stride_W,
	int dilate_H,
	int dilate_W,
	zq_base_type* out_tensor4D_data,
	int out_N,	// must be in_N
	int out_H,	// must be (in_H - filter_H)/stride_H + 1
	int out_W,	// must be (in_W - filter_W)/stride_W + 1
	int out_C,	// must be in_C
	int out_widthStep,
	int out_sliceStep,
	int out_imStep
#if WITH_BIAS
	, const zq_base_type* bias
#endif
#if WITH_PRELU
	, const zq_base_type* slope
#endif
)
{
	const zq_base_type* in_slice_ptr;
	const zq_base_type* in_row_ptr;
	const zq_base_type* in_pix_ptr;
	const zq_base_type* in_im_ptr;
	const zq_base_type* filter_slice_ptr;
	zq_base_type* out_slice_ptr;
	zq_base_type* out_row_ptr;
	zq_base_type* out_pix_ptr;
	zq_base_type* out_im_ptr;

	const zq_base_type* cur_in_row_ptr0, *cur_in_row_ptr1, *cur_in_row_ptr2;
	const zq_base_type* cur_in_pix_ptr0, *cur_in_pix_ptr1, *cur_in_pix_ptr2;
	const zq_base_type* cur_filter_row_ptr;
	register zq_mm_type value_v0, value_v1, value_v2;
	register zq_mm_type a0, b0, c0;
	register zq_mm_type a1, b1, c1;
	register zq_mm_type a2, b2, c2;
	register zq_mm_type w00, w01, w02, w10, w11, w12, w20, w21, w22;
	int in_widthStep2 = in_widthStep * 2;
#if WITH_BIAS
	register zq_mm_type bias_v;
#endif
#if WITH_PRELU
	register zq_mm_type slope_v;
	register zq_mm_type zero_v = zq_mm_setzero_ps();
#endif
	int out_n, out_h, out_w, out_c;

	if (out_W % 3 == 0)
	{
		for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
			out_n < out_N;
			out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
		{
			for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
				out_c < in_C;
				out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
			{
#if WITH_BIAS
				bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
				slope_v = zq_mm_load_ps(slope + out_c);
#endif
				cur_filter_row_ptr = filter_slice_ptr;
				w00 = zq_mm_load_ps(cur_filter_row_ptr);
				w01 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w02 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w10 = zq_mm_load_ps(cur_filter_row_ptr);
				w11 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w12 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w20 = zq_mm_load_ps(cur_filter_row_ptr);
				w21 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w22 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);

				for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
					out_h < out_H;
					out_h++, in_row_ptr += in_widthStep, out_row_ptr += out_widthStep)
				{
					out_pix_ptr = out_row_ptr;
					cur_in_row_ptr0 = in_row_ptr;
					cur_in_row_ptr1 = in_row_ptr + in_widthStep;
					cur_in_row_ptr2 = in_row_ptr + in_widthStep2;
					a0 = zq_mm_load_ps(cur_in_row_ptr0);
					a1 = zq_mm_load_ps(cur_in_row_ptr1);
					a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
					value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
					value_v0 = zq_mm_mul_ps(a0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
					value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					b0 = zq_mm_load_ps(cur_in_row_ptr0);
					b1 = zq_mm_load_ps(cur_in_row_ptr1);
					b2 = zq_mm_load_ps(cur_in_row_ptr2);
					value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
					value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
					value_v1 = zq_mm_mul_ps(b0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
					value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
					value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
					value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);
					for (out_w = 0; out_w < out_W; out_w += 3)
					{
						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						c0 = zq_mm_load_ps(cur_in_row_ptr0);
						c1 = zq_mm_load_ps(cur_in_row_ptr1);
						c2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(c0, w02, value_v0);
						value_v1 = zq_mm_fmadd_ps(c0, w01, value_v1);
#if WITH_BIAS
						value_v2 = zq_mm_fmadd_ps(c0, w00, bias_v);
#else
						value_v2 = zq_mm_mul_ps(c0, w00);
#endif
						value_v0 = zq_mm_fmadd_ps(c1, w12, value_v0);
						value_v1 = zq_mm_fmadd_ps(c1, w11, value_v1);
						value_v2 = zq_mm_fmadd_ps(c1, w10, value_v2);
						value_v0 = zq_mm_fmadd_ps(c2, w22, value_v0);
						value_v1 = zq_mm_fmadd_ps(c2, w21, value_v1);
						value_v2 = zq_mm_fmadd_ps(c2, w20, value_v2);
#if WITH_PRELU
						value_v0 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v0), zq_mm_max_ps(zero_v, value_v0));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v0);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						a0 = zq_mm_load_ps(cur_in_row_ptr0);
						a1 = zq_mm_load_ps(cur_in_row_ptr1);
						a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
						value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
						value_v0 = zq_mm_mul_ps(a0, w00);
#endif
						value_v1 = zq_mm_fmadd_ps(a0, w02, value_v1);
						value_v2 = zq_mm_fmadd_ps(a0, w01, value_v2);
						value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
						value_v1 = zq_mm_fmadd_ps(a1, w12, value_v1);
						value_v2 = zq_mm_fmadd_ps(a1, w11, value_v2);
						value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
						value_v1 = zq_mm_fmadd_ps(a2, w22, value_v1);
						value_v2 = zq_mm_fmadd_ps(a2, w21, value_v2);
#if WITH_PRELU
						value_v1 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v1), zq_mm_max_ps(zero_v, value_v1));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v1);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						b0 = zq_mm_load_ps(cur_in_row_ptr0);
						b1 = zq_mm_load_ps(cur_in_row_ptr1);
						b2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
						value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
						value_v1 = zq_mm_mul_ps(b0, w00);
#endif
						value_v2 = zq_mm_fmadd_ps(b0, w02, value_v2);
						value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
						value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
						value_v2 = zq_mm_fmadd_ps(b1, w12, value_v2);
						value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
						value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);
						value_v2 = zq_mm_fmadd_ps(b2, w22, value_v2);
#if WITH_PRELU
						value_v2 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v2), zq_mm_max_ps(zero_v, value_v2));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v2);
						out_pix_ptr += zq_mm_align_size;
					}
				}
			}
		}
	}
	else if (out_W % 3 == 1)
	{
		for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
			out_n < out_N;
			out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
		{
			for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
				out_c < in_C;
				out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
			{
#if WITH_BIAS
				bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
				slope_v = zq_mm_load_ps(slope + out_c);
#endif
				cur_filter_row_ptr = filter_slice_ptr;
				w00 = zq_mm_load_ps(cur_filter_row_ptr);
				w01 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w02 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w10 = zq_mm_load_ps(cur_filter_row_ptr);
				w11 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w12 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w20 = zq_mm_load_ps(cur_filter_row_ptr);
				w21 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w22 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);

				for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
					out_h < out_H;
					out_h++, in_row_ptr += in_widthStep, out_row_ptr += out_widthStep)
				{
					out_pix_ptr = out_row_ptr;
					cur_in_row_ptr0 = in_row_ptr;
					cur_in_row_ptr1 = in_row_ptr + in_widthStep;
					cur_in_row_ptr2 = in_row_ptr + in_widthStep2;
					a0 = zq_mm_load_ps(cur_in_row_ptr0);
					a1 = zq_mm_load_ps(cur_in_row_ptr1);
					a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
					value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
					value_v0 = zq_mm_mul_ps(a0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
					value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					b0 = zq_mm_load_ps(cur_in_row_ptr0);
					b1 = zq_mm_load_ps(cur_in_row_ptr1);
					b2 = zq_mm_load_ps(cur_in_row_ptr2);
					value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
					value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
					value_v1 = zq_mm_mul_ps(b0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
					value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
					value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
					value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);

					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					c0 = zq_mm_load_ps(cur_in_row_ptr0);
					c1 = zq_mm_load_ps(cur_in_row_ptr1);
					c2 = zq_mm_load_ps(cur_in_row_ptr2);
					value_v0 = zq_mm_fmadd_ps(c0, w02, value_v0);
					value_v1 = zq_mm_fmadd_ps(c0, w01, value_v1);
#if WITH_BIAS
					value_v2 = zq_mm_fmadd_ps(c0, w00, bias_v);
#else
					value_v2 = zq_mm_mul_ps(c0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(c1, w12, value_v0);
					value_v1 = zq_mm_fmadd_ps(c1, w11, value_v1);
					value_v2 = zq_mm_fmadd_ps(c1, w10, value_v2);
					value_v0 = zq_mm_fmadd_ps(c2, w22, value_v0);
					value_v1 = zq_mm_fmadd_ps(c2, w21, value_v1);
					value_v2 = zq_mm_fmadd_ps(c2, w20, value_v2);
#if WITH_PRELU
					value_v0 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v0), zq_mm_max_ps(zero_v, value_v0));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v0);
					out_pix_ptr += zq_mm_align_size;

					for (out_w = 1; out_w < out_W; out_w += 3)
					{
						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						a0 = zq_mm_load_ps(cur_in_row_ptr0);
						a1 = zq_mm_load_ps(cur_in_row_ptr1);
						a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
						value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
						value_v0 = zq_mm_mul_ps(a0, w00);
#endif
						value_v1 = zq_mm_fmadd_ps(a0, w02, value_v1);
						value_v2 = zq_mm_fmadd_ps(a0, w01, value_v2);
						value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
						value_v1 = zq_mm_fmadd_ps(a1, w12, value_v1);
						value_v2 = zq_mm_fmadd_ps(a1, w11, value_v2);
						value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
						value_v1 = zq_mm_fmadd_ps(a2, w22, value_v1);
						value_v2 = zq_mm_fmadd_ps(a2, w21, value_v2);
#if WITH_PRELU
						value_v1 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v1), zq_mm_max_ps(zero_v, value_v1));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v1);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						b0 = zq_mm_load_ps(cur_in_row_ptr0);
						b1 = zq_mm_load_ps(cur_in_row_ptr1);
						b2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
						value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
						value_v1 = zq_mm_mul_ps(b0, w00);
#endif
						value_v2 = zq_mm_fmadd_ps(b0, w02, value_v2);
						value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
						value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
						value_v2 = zq_mm_fmadd_ps(b1, w12, value_v2);
						value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
						value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);
						value_v2 = zq_mm_fmadd_ps(b2, w22, value_v2);
#if WITH_PRELU
						value_v2 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v2), zq_mm_max_ps(zero_v, value_v2));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v2);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						c0 = zq_mm_load_ps(cur_in_row_ptr0);
						c1 = zq_mm_load_ps(cur_in_row_ptr1);
						c2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(c0, w02, value_v0);
						value_v1 = zq_mm_fmadd_ps(c0, w01, value_v1);
#if WITH_BIAS
						value_v2 = zq_mm_fmadd_ps(c0, w00, bias_v);
#else
						value_v2 = zq_mm_mul_ps(c0, w00);
#endif
						value_v0 = zq_mm_fmadd_ps(c1, w12, value_v0);
						value_v1 = zq_mm_fmadd_ps(c1, w11, value_v1);
						value_v2 = zq_mm_fmadd_ps(c1, w10, value_v2);
						value_v0 = zq_mm_fmadd_ps(c2, w22, value_v0);
						value_v1 = zq_mm_fmadd_ps(c2, w21, value_v1);
						value_v2 = zq_mm_fmadd_ps(c2, w20, value_v2);
#if WITH_PRELU
						value_v0 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v0), zq_mm_max_ps(zero_v, value_v0));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v0);
						out_pix_ptr += zq_mm_align_size;
					}
				}
			}
		}
	}
	else //if (out_W % 3 == 2)
	{
		for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
			out_n < out_N;
			out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
		{
			for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
				out_c < in_C;
				out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
			{
#if WITH_BIAS
				bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
				slope_v = zq_mm_load_ps(slope + out_c);
#endif
				cur_filter_row_ptr = filter_slice_ptr;
				w00 = zq_mm_load_ps(cur_filter_row_ptr);
				w01 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w02 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w10 = zq_mm_load_ps(cur_filter_row_ptr);
				w11 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w12 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);
				cur_filter_row_ptr += filter_widthStep;
				w20 = zq_mm_load_ps(cur_filter_row_ptr);
				w21 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size);
				w22 = zq_mm_load_ps(cur_filter_row_ptr + zq_mm_align_size2);

				for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
					out_h < out_H;
					out_h++, in_row_ptr += in_widthStep, out_row_ptr += out_widthStep)
				{
					out_pix_ptr = out_row_ptr;
					cur_in_row_ptr0 = in_row_ptr;
					cur_in_row_ptr1 = in_row_ptr + in_widthStep;
					cur_in_row_ptr2 = in_row_ptr + in_widthStep2;
					a0 = zq_mm_load_ps(cur_in_row_ptr0);
					a1 = zq_mm_load_ps(cur_in_row_ptr1);
					a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
					value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
					value_v0 = zq_mm_mul_ps(a0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
					value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					b0 = zq_mm_load_ps(cur_in_row_ptr0);
					b1 = zq_mm_load_ps(cur_in_row_ptr1);
					b2 = zq_mm_load_ps(cur_in_row_ptr2);
					value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
					value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
					value_v1 = zq_mm_mul_ps(b0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
					value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
					value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
					value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);

					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					c0 = zq_mm_load_ps(cur_in_row_ptr0);
					c1 = zq_mm_load_ps(cur_in_row_ptr1);
					c2 = zq_mm_load_ps(cur_in_row_ptr2);
					value_v0 = zq_mm_fmadd_ps(c0, w02, value_v0);
					value_v1 = zq_mm_fmadd_ps(c0, w01, value_v1);
#if WITH_BIAS
					value_v2 = zq_mm_fmadd_ps(c0, w00, bias_v);
#else
					value_v2 = zq_mm_mul_ps(c0, w00);
#endif
					value_v0 = zq_mm_fmadd_ps(c1, w12, value_v0);
					value_v1 = zq_mm_fmadd_ps(c1, w11, value_v1);
					value_v2 = zq_mm_fmadd_ps(c1, w10, value_v2);
					value_v0 = zq_mm_fmadd_ps(c2, w22, value_v0);
					value_v1 = zq_mm_fmadd_ps(c2, w21, value_v1);
					value_v2 = zq_mm_fmadd_ps(c2, w20, value_v2);
#if WITH_PRELU
					value_v0 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v0), zq_mm_max_ps(zero_v, value_v0));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v0);
					out_pix_ptr += zq_mm_align_size;

					cur_in_row_ptr0 += zq_mm_align_size;
					cur_in_row_ptr1 += zq_mm_align_size;
					cur_in_row_ptr2 += zq_mm_align_size;
					a0 = zq_mm_load_ps(cur_in_row_ptr0);
					a1 = zq_mm_load_ps(cur_in_row_ptr1);
					a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
					value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
					value_v0 = zq_mm_mul_ps(a0, w00);
#endif
					value_v1 = zq_mm_fmadd_ps(a0, w02, value_v1);
					value_v2 = zq_mm_fmadd_ps(a0, w01, value_v2);
					value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
					value_v1 = zq_mm_fmadd_ps(a1, w12, value_v1);
					value_v2 = zq_mm_fmadd_ps(a1, w11, value_v2);
					value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
					value_v1 = zq_mm_fmadd_ps(a2, w22, value_v1);
					value_v2 = zq_mm_fmadd_ps(a2, w21, value_v2);
#if WITH_PRELU
					value_v1 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v1), zq_mm_max_ps(zero_v, value_v1));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v1);
					out_pix_ptr += zq_mm_align_size;

					for (out_w = 2; out_w < out_W; out_w += 3)
					{
						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						b0 = zq_mm_load_ps(cur_in_row_ptr0);
						b1 = zq_mm_load_ps(cur_in_row_ptr1);
						b2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(b0, w01, value_v0);
#if WITH_BIAS
						value_v1 = zq_mm_fmadd_ps(b0, w00, bias_v);
#else
						value_v1 = zq_mm_mul_ps(b0, w00);
#endif
						value_v2 = zq_mm_fmadd_ps(b0, w02, value_v2);
						value_v0 = zq_mm_fmadd_ps(b1, w11, value_v0);
						value_v1 = zq_mm_fmadd_ps(b1, w10, value_v1);
						value_v2 = zq_mm_fmadd_ps(b1, w12, value_v2);
						value_v0 = zq_mm_fmadd_ps(b2, w21, value_v0);
						value_v1 = zq_mm_fmadd_ps(b2, w20, value_v1);
						value_v2 = zq_mm_fmadd_ps(b2, w22, value_v2);
#if WITH_PRELU
						value_v2 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v2), zq_mm_max_ps(zero_v, value_v2));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v2);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						c0 = zq_mm_load_ps(cur_in_row_ptr0);
						c1 = zq_mm_load_ps(cur_in_row_ptr1);
						c2 = zq_mm_load_ps(cur_in_row_ptr2);
						value_v0 = zq_mm_fmadd_ps(c0, w02, value_v0);
						value_v1 = zq_mm_fmadd_ps(c0, w01, value_v1);
#if WITH_BIAS
						value_v2 = zq_mm_fmadd_ps(c0, w00, bias_v);
#else
						value_v2 = zq_mm_mul_ps(c0, w00);
#endif
						value_v0 = zq_mm_fmadd_ps(c1, w12, value_v0);
						value_v1 = zq_mm_fmadd_ps(c1, w11, value_v1);
						value_v2 = zq_mm_fmadd_ps(c1, w10, value_v2);
						value_v0 = zq_mm_fmadd_ps(c2, w22, value_v0);
						value_v1 = zq_mm_fmadd_ps(c2, w21, value_v1);
						value_v2 = zq_mm_fmadd_ps(c2, w20, value_v2);
#if WITH_PRELU
						value_v0 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v0), zq_mm_max_ps(zero_v, value_v0));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v0);
						out_pix_ptr += zq_mm_align_size;

						cur_in_row_ptr0 += zq_mm_align_size;
						cur_in_row_ptr1 += zq_mm_align_size;
						cur_in_row_ptr2 += zq_mm_align_size;
						a0 = zq_mm_load_ps(cur_in_row_ptr0);
						a1 = zq_mm_load_ps(cur_in_row_ptr1);
						a2 = zq_mm_load_ps(cur_in_row_ptr2);
#if WITH_BIAS
						value_v0 = zq_mm_fmadd_ps(a0, w00, bias_v);
#else
						value_v0 = zq_mm_mul_ps(a0, w00);
#endif
						value_v1 = zq_mm_fmadd_ps(a0, w02, value_v1);
						value_v2 = zq_mm_fmadd_ps(a0, w01, value_v2);
						value_v0 = zq_mm_fmadd_ps(a1, w10, value_v0);
						value_v1 = zq_mm_fmadd_ps(a1, w12, value_v1);
						value_v2 = zq_mm_fmadd_ps(a1, w11, value_v2);
						value_v0 = zq_mm_fmadd_ps(a2, w20, value_v0);
						value_v1 = zq_mm_fmadd_ps(a2, w22, value_v1);
						value_v2 = zq_mm_fmadd_ps(a2, w21, value_v2);
#if WITH_PRELU
						value_v1 = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v1), zq_mm_max_ps(zero_v, value_v1));
#endif
						zq_mm_store_ps(out_pix_ptr, value_v1);
						out_pix_ptr += zq_mm_align_size;
					}
				}
			}
		}
	}
}


void zq_cnn_depthwise_conv_no_padding_nchwc_kernel2x2(
	const zq_base_type* in_tensor4D_data,
	int in_N,
	int in_H,
	int in_W,
	int in_C,
	int in_widthStep,
	int in_sliceStep,
	int in_imStep,
	const zq_base_type* filters_data,
	int filter_N, //must be 1
	int filter_H,
	int filter_W,
	int filter_C, // must be in_C
	int filter_widthStep,
	int filter_sliceStep,
	int filter_imStep,
	int stride_H,
	int stride_W,
	int dilate_H,
	int dilate_W,
	zq_base_type* out_tensor4D_data,
	int out_N,	// must be in_N
	int out_H,	// must be (in_H - filter_H)/stride_H + 1
	int out_W,	// must be (in_W - filter_W)/stride_W + 1
	int out_C,	// must be in_C
	int out_widthStep,
	int out_sliceStep,
	int out_imStep
#if WITH_BIAS
	, const zq_base_type* bias
#endif
#if WITH_PRELU
	, const zq_base_type* slope
#endif
)
{
	const zq_base_type* in_slice_ptr;
	const zq_base_type* in_row_ptr;
	const zq_base_type* in_pix_ptr;
	const zq_base_type* in_im_ptr;
	const zq_base_type* filter_slice_ptr;
	zq_base_type* out_slice_ptr;
	zq_base_type* out_row_ptr;
	zq_base_type* out_pix_ptr;
	zq_base_type* out_im_ptr;

	const zq_base_type* cur_in_row_ptr;
	const zq_base_type* cur_in_pix_ptr;
	const zq_base_type* cur_filter_row_ptr;
	const zq_base_type* cur_filter_pix_ptr;
	register zq_mm_type value_v;
#if WITH_BIAS
	register zq_mm_type bias_v;
#endif
#if WITH_PRELU
	register zq_mm_type slope_v;
	register zq_mm_type zero_v = zq_mm_setzero_ps();
#endif
	int stride_H_mul_in_widthStep = stride_H*in_widthStep;
	int stride_W_mul_in_pixStep = stride_W*zq_mm_align_size;
	int dilate_H_mul_in_widthStep = dilate_H*in_widthStep;
	int dilate_W_mul_in_pixStep = dilate_W*zq_mm_align_size;
	int out_n, out_h, out_w, out_c;

	for (out_n = 0, in_im_ptr = in_tensor4D_data, out_im_ptr = out_tensor4D_data;
		out_n < out_N;
		out_n++, in_im_ptr += in_imStep, out_im_ptr += out_imStep)
	{
		for (out_c = 0, in_slice_ptr = in_im_ptr, out_slice_ptr = out_im_ptr, filter_slice_ptr = filters_data;
			out_c < in_C;
			out_c += zq_mm_align_size, in_slice_ptr += in_sliceStep, out_slice_ptr += out_sliceStep, filter_slice_ptr += filter_sliceStep)
		{
#if WITH_BIAS
			bias_v = zq_mm_load_ps(bias + out_c);
#endif
#if WITH_PRELU
			slope_v = zq_mm_load_ps(slope + out_c);
#endif
			for (out_h = 0, in_row_ptr = in_slice_ptr, out_row_ptr = out_slice_ptr;
				out_h < out_H;
				out_h++, in_row_ptr += stride_H_mul_in_widthStep, out_row_ptr += out_widthStep)
			{
				for (out_w = 0, in_pix_ptr = in_row_ptr, out_pix_ptr = out_row_ptr;
					out_w < out_W;
					out_w++, in_pix_ptr += stride_W_mul_in_pixStep, out_pix_ptr += zq_mm_align_size)
				{
					cur_in_row_ptr = in_pix_ptr; cur_filter_row_ptr = filter_slice_ptr;
					cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
#if WITH_BIAS
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), bias_v);
#else
					value_v = zq_mm_mul_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr));
#endif
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);

					cur_in_row_ptr += dilate_H_mul_in_widthStep; cur_filter_row_ptr += filter_widthStep;
					cur_in_pix_ptr = cur_in_row_ptr, cur_filter_pix_ptr = cur_filter_row_ptr;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);
					cur_in_pix_ptr += dilate_W_mul_in_pixStep; cur_filter_pix_ptr += zq_mm_align_size;
					value_v = zq_mm_fmadd_ps(zq_mm_load_ps(cur_in_pix_ptr), zq_mm_load_ps(cur_filter_pix_ptr), value_v);

#if WITH_PRELU
					value_v = zq_mm_fmadd_ps(slope_v, zq_mm_min_ps(zero_v, value_v), zq_mm_max_ps(zero_v, value_v));
#endif
					zq_mm_store_ps(out_pix_ptr, value_v);
				}
			}
		}
	}
}