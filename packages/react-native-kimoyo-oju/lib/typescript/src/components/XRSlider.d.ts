import type { XRInteractiveProps, SliderOrientation } from './types';
/**
 * XRSlider Props
 */
export interface XRSliderProps extends XRInteractiveProps {
    value?: number;
    defaultValue?: number;
    min?: number;
    max?: number;
    step?: number;
    orientation?: SliderOrientation;
    width?: number;
    showValue?: boolean;
    formatValue?: (value: number) => string;
    onChange?: (value: number) => void;
    onChangeEnd?: (value: number) => void;
}
/**
 * XRSlider - Interactive slider for value selection
 *
 * Supports both horizontal and vertical orientations.
 * Works with near and ray interactions.
 *
 * @example
 * ```tsx
 * <XRSlider
 *   min={0}
 *   max={100}
 *   value={volume}
 *   onChange={setVolume}
 *   showValue
 * />
 * ```
 */
export declare function XRSlider({ position, rotation, scale, visible, opacity, onMount, onUnmount, disabled, onHover, onHoverEnd, onSelect, onSelectStart, onSelectEnd, value: controlledValue, defaultValue, min, max, step, orientation, width, showValue, formatValue, onChange, onChangeEnd, }: XRSliderProps): null;
export declare namespace XRSlider {
    var displayName: string;
}
/**
 * XRRangeSlider - Slider with two thumbs for range selection
 */
export interface XRRangeSliderProps extends Omit<XRSliderProps, 'value' | 'defaultValue' | 'onChange' | 'onChangeEnd'> {
    value?: [number, number];
    defaultValue?: [number, number];
    onChange?: (value: [number, number]) => void;
    onChangeEnd?: (value: [number, number]) => void;
    minDistance?: number;
}
export declare function XRRangeSlider({ defaultValue, minDistance, ...props }: XRRangeSliderProps): null;
export declare namespace XRRangeSlider {
    var displayName: string;
}
//# sourceMappingURL=XRSlider.d.ts.map